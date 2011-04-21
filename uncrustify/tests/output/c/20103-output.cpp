/**
 * @file output.cpp
 * Does all the output & comment formatting.
 *
 * $Id: output.cpp 510 2006-09-20 01:14:56Z bengardner $
 */

#include "uncrustify_types.h"
#include "prototypes.h"
#include "chunk_list.h"
#include <cstring>
#include <cstdlib>



void add_char(char ch)
  {
    /* convert a newline into the LF/CRLF/CR sequence */
  if (ch == '\n')
    {
    fputs(cpd.newline, cpd.fout);
    cpd.column      = 1;
    cpd.did_newline = 1;
    }
  else
    {
    fputc(ch, cpd.fout);

    if (ch == '\t')
      cpd.column = next_tab_column(cpd.column);
    else
      cpd.column++;
    }
  }

void add_text(const char *text)
  {
  char ch;

  while ((ch = *text) != 0)
    {
    text++;
    add_char(ch);
    }
  }

void add_text_len(const char *text, int len)
  {
  while (len-- > 0)
    {
    add_char(*text);
    text++;
    }
  }

/**
 * Advance to a specific column
 * cpd.column is the current column
 *
 * @param column  The column to advance to
 */
void output_to_column(int column, bool allow_tabs)
  {
  int nc;

  cpd.did_newline = 0;

  if (allow_tabs)
    {
      /* tab out as far as possible and then use spaces */
    while ((nc = next_tab_column(cpd.column)) <= column)
      add_text("\t");
    }

    /* space out the final bit */
  while (cpd.column < column)
    add_text(" ");
  }

void output_indent(int column, int brace_col)
  {
  if ((cpd.column == 1) && (cpd.settings[UO_indent_with_tabs].n != 0))
    {
    if (cpd.settings[UO_indent_with_tabs].n == 2)
      brace_col = column;

      /* tab out as far as possible and then use spaces */
    int nc;

    while ((nc = next_tab_column(cpd.column)) <= brace_col)
      add_text("\t");
    }

    /* space out the rest */
  while (cpd.column < column)
    add_text(" ");
  }

void output_parsed(FILE *pfile)
  {
  chunk_t *pc;
  int cnt;

  output_options(pfile);
  output_defines(pfile);
  output_types(pfile);

  fprintf(pfile, "-=====-\n");
  fprintf(pfile, "Line      Tag          Parent     Columns  Br/Lvl/pp Flg Nl  Text");

  for (pc = chunk_get_head(); pc != NULL; pc = chunk_get_next(pc))
    {
    fprintf(pfile, "\n%3d> %13.13s[%13.13s][%2d/%2d/%2d][%d/%d/%d][%6x][%d-%d]",
            pc->orig_line, get_token_name(pc->type),
            get_token_name(pc->parent_type),
            pc->column, pc->orig_col, pc->orig_col_end,
            pc->brace_level, pc->level, pc->pp_level,
            pc->flags, pc->nl_count, pc->after_tab);

    if ((pc->type != CT_NEWLINE) && (pc->len != 0))
      {
      for (cnt = 0; cnt < pc->column; cnt++)
        fprintf(pfile, " ");

      fprintf(pfile, "%.*s", pc->len, pc->str);
      }
    }

  fprintf(pfile, "\n-=====-\n");
  fflush(pfile);
  }

void output_options(FILE *pfile)
  {
  int idx;
  const option_map_value *ptr;

  fprintf(pfile, "-== Options ==-\n");

  for (idx = 0; idx < UO_option_count; idx++)
    {
    ptr = get_option_name(idx);

    if (ptr != NULL)
      {
      if (ptr->type == AT_BOOL)
        {
        fprintf(pfile, "%3d) %32s = %s\n",
                ptr->id, ptr->name,
                cpd.settings[ptr->id].b ? "True" : "False");
        }
      else if (ptr->type == AT_IARF)
        {
        fprintf(pfile, "%3d) %32s = %s\n",
                ptr->id, ptr->name,
                (cpd.settings[ptr->id].a == AV_ADD) ? "Add" :
                (cpd.settings[ptr->id].a == AV_REMOVE) ? "Remove" :
                (cpd.settings[ptr->id].a == AV_FORCE) ? "Force" : "Ignore");
        }
      else if (ptr->type == AT_LINE)
        {
        fprintf(pfile, "%3d) %32s = %s\n",
                ptr->id, ptr->name,
                (cpd.settings[ptr->id].le == LE_AUTO) ? "Auto" :
                (cpd.settings[ptr->id].le == LE_LF) ? "LF" :
                (cpd.settings[ptr->id].le == LE_CRLF) ? "CRLF" :
                (cpd.settings[ptr->id].le == LE_CR) ? "CR" : "???");
        }
      else /* AT_NUM */
        fprintf(pfile, "%3d) %32s = %d\n",
                ptr->id, ptr->name, cpd.settings[ptr->id].n);
      }
    }
  }

/**
 * This renders the chunk list to a file.
 */
void output_text(FILE *pfile)
  {
  chunk_t *pc;
  chunk_t *prev;
  int cnt;
  int lvlcol;
  bool allow_tabs;

  cpd.fout = pfile;

  for (pc = chunk_get_head(); pc != NULL; pc = chunk_get_next(pc))
    {
    if (pc->type == CT_NEWLINE)
      {
      for (cnt = 0; cnt < pc->nl_count; cnt++)
        add_char('\n');

      cpd.did_newline = 1;
      cpd.column      = 1;
      LOG_FMT(LOUTIND, " xx\n");
      }
    else if (pc->type == CT_COMMENT_MULTI)
      output_comment_multi(pc);
    else if (pc->type == CT_COMMENT_CPP)
      pc = output_comment_cpp(pc);
    else if (pc->len == 0)
        /* don't do anything for non-visible stuff */
      LOG_FMT(LOUTIND, " <%d> -", pc->column);
    else
      {
        /* indent to the 'level' first */
      if (cpd.did_newline)
        {
        if (cpd.settings[UO_indent_with_tabs].n == 1)
          {
          lvlcol = 1 + (pc->brace_level * cpd.settings[UO_indent_columns].n);

          if ((pc->column >= lvlcol) && (lvlcol > 1))
            output_to_column(lvlcol, true);
          }

        allow_tabs = (cpd.settings[UO_indent_with_tabs].n == 2) ||
                     (chunk_is_comment(pc) &&
                      (cpd.settings[UO_indent_with_tabs].n != 0));

        LOG_FMT(LOUTIND, "  %d> col %d/%d - ", pc->orig_line, pc->column, cpd.column);
        }
      else
        {
          /* not the first item on a line */
        if (cpd.settings[UO_align_keep_tabs].b)
          allow_tabs = pc->after_tab;
        else
          {
          prev       = chunk_get_prev(pc);
          allow_tabs = (cpd.settings[UO_align_with_tabs].b &&
                        ((pc->flags & PCF_WAS_ALIGNED) != 0) &&
                        (((pc->column - 1) % cpd.settings[UO_output_tab_size].n) == 0) &&
                        ((prev->column + prev->len + 1) != pc->column));
          }

        LOG_FMT(LOUTIND, " %d -", pc->column);
        }

      output_to_column(pc->column, allow_tabs);
      add_text_len(pc->str, pc->len);
      cpd.did_newline = chunk_is_newline(pc);
      }
    }
  }

/**
 * Given a multi-line comemnt block that starts in column X, figure out how
 * much subsequent lines should be indented.
 *
 * The answer is either 0 or 1.
 *
 * The decision is based on:
 *  - the first line length
 *  - the second line leader length
 *  - the last line length
 *
 * If the first and last line are the same length and don't contain any alnum
 * chars and (the first line len > 2 or the second leader is the same as the
 * first line length), then the indent is 0.
 *
 * If the leader on the second line is 1 wide or missing, then the indent is 1.
 *
 * Otherwise, the indent is 0.
 *
 * @param str       The comment string
 * @param len       Length of the comment
 * @param start_col Starting column
 * @return 0 or 1
 */
static int calculate_comment_body_indent(const char *str, int len, int start_col)
  {
  int idx       = 0;
  int first_len = 0;
  int last_len  = 0;
  int width     = 0;

    /* find the last line length */
  for (idx = len - 1; idx > 0; idx--)
    {
    if ((str[idx] == '\n') || (str[idx] == '\r'))
      {
      idx++;

      while ((idx < len) && ((str[idx] == ' ') || (str[idx] == '\t')))
        idx++;

      last_len = len - idx;
      break;
      }
    }

    /* find the first line length */
  for (idx = 0; idx < len; idx++)
    {
    if ((str[idx] == '\n') || (str[idx] == '\r'))
      {
      first_len = idx;

      while ((str[first_len - 1] == ' ') || (str[first_len - 1] == '\t'))
        first_len--;

        /* handle DOS endings */
      if ((str[idx] == '\r') && (str[idx + 1] == '\n'))
        idx++;

      idx++;
      break;
      }
    }

    /* Scan the second line */
  width = 0;

  for (/* nada */; idx < len; idx++)
    {
    if ((str[idx] == ' ') || (str[idx] == '\t'))
      {
      if (width > 0)
        break;

      continue;
      }

    if ((str[idx] == '\n') || (str[idx] == '\r'))
        /* Done with second line */
      break;

      /* Count the leading chars */
    if ((str[idx] == '*') ||
        (str[idx] == '|') ||
        (str[idx] == '\\') ||
        (str[idx] == '#') ||
        (str[idx] == '+'))
      width++;
    else
      break;
    }

  //LOG_FMT(LSYS, "%s: first=%d last=%d width=%d\n", __func__, first_len, last_len, width);

    /*TODO: make the first_len minimum (4) configurable? */
  if ((first_len == last_len) && ((first_len > 4) || first_len == width))
    return 0;

  return (width == 2) ? 0 : 1;
  }

/**
 * Outputs the CPP comment at pc.
 * CPP comment combining is done here
 *
 * @return the last chunk output'd
 */
chunk_t *output_comment_cpp(chunk_t *first)
  {
  int col    = first->column;
  int col_br = 1 + (first->brace_level * cpd.settings[UO_indent_columns].n);

    /* Make sure we have at least one space past the last token */
  if (first->parent_type == CT_COMMENT_END)
    {
    chunk_t *prev = chunk_get_prev(first);

    if (prev != NULL)
      {
      int col_min = prev->column + prev->len + 1;

      if (col < col_min)
        col = col_min;
      }
    }

    /* Bump out to the column */
  output_indent(col, col_br);

  if (!cpd.settings[UO_cmt_cpp_to_c].b)
    {
    add_text_len(first->str, first->len);
    return first;
    }

    /* If we are grouping, see if there is something to group */
  bool combined = false;

  if (cpd.settings[UO_cmt_cpp_group].b)
    {
      /* next is a newline by definition */
    chunk_t *next = chunk_get_next(first);

    if ((next != NULL) && (next->nl_count == 1))
      {
      next = chunk_get_next(next);

      /**
       * Only combine the next comment if they are both at indent level or
       * the second one is NOT at indent or less
       *
       * A trailing comment cannot be combined with a comment at indent
       * level or less
       */
      if ((next != NULL) &&
          (next->type == CT_COMMENT_CPP) &&
          (((next->column == 1) && (first->column == 1)) ||
           ((next->column == col_br) && (first->column == col_br)) ||
           ((next->column > col_br) && (first->parent_type == CT_COMMENT_END))))
        combined = true;
      }
    }

  if (!combined)
    {
      /* nothing to group: just output a single line */
    add_text_len("/*", 2);

    if ((first->str[2] != ' ') && (first->str[2] != '\t'))
      add_char(' ');

    add_text_len(&first->str[2], first->len - 2);
    add_text_len(" */", 3);
    return first;
    }

  chunk_t *pc   = first;
  chunk_t *last = first;

    /* Output the first line */
  add_text_len("/*", 2);

  if (combined && cpd.settings[UO_cmt_cpp_nl_start].b)
    /* I suppose someone more clever could do this without a goto or
     * repeating too much code...
     */
    goto cpp_newline;

  goto cpp_addline;

    /* Output combined lines */
  while ((pc = chunk_get_next(pc)) != NULL)
    {
    if ((pc->type == CT_NEWLINE) && (pc->nl_count == 1))
      continue;

    if (pc->type != CT_COMMENT_CPP)
      break;

    if (((pc->column == 1) && (first->column == 1)) ||
        ((pc->column == col_br) && (first->column == col_br)) ||
        ((pc->column > col_br) && (first->parent_type == CT_COMMENT_END)))
      {
      last = pc;
cpp_newline:
      add_char('\n');
      output_indent(col, col_br);
      add_char(' ');
      add_char(cpd.settings[UO_cmt_star_cont].b ? '*' : ' ');
cpp_addline:

      if ((pc->str[2] != ' ') && (pc->str[2] != '\t'))
        add_char(' ');

      add_text_len(&pc->str[2], pc->len - 2);
      }
    }

  if (cpd.settings[UO_cmt_cpp_nl_end].b)
    {
    add_char('\n');
    output_indent(col, col_br);
    }

  add_text_len(" */", 3);
  return last;
  }

void output_comment_multi(chunk_t *pc)
  {
  int cmt_col = pc->column;
  const char *cmt_str;
  int remaining;
  char ch;
  chunk_t    *prev;
  char line[1024];
  int line_len;
  int line_count = 0;
  int ccol;
  int col_diff = 0;
  int xtra     = 1;

  prev = chunk_get_prev(pc);

  if ((prev != NULL) && (prev->type != CT_NEWLINE))
    cmt_col = pc->orig_col;
  else
    col_diff = pc->orig_col - pc->column;

  //   fprintf(stderr, "Indenting1 line %d to col %d (orig=%d) col_diff=%d\n",
  //           pc->orig_line, cmt_col, pc->orig_col, col_diff);

  xtra = calculate_comment_body_indent(pc->str, pc->len, pc->column);

  ccol      = 1;
  remaining = pc->len;
  cmt_str   = pc->str;
  line_len  = 0;

  while (remaining > 0)
    {
    ch = *cmt_str;
    cmt_str++;
    remaining--;

      /* handle the CRLF and CR endings. convert both to LF */
    if (ch == '\r')
      {
      ch = '\n';

      if (*cmt_str == '\n')
        {
        cmt_str++;
        remaining--;
        }
      }

      /* Find the start column */
    if (line_len == 0)
      {
      if (ch == ' ')
        {
        ccol++;
        continue;
        }
      else if (ch == '\t')
        {
        ccol = calc_next_tab_column(ccol, cpd.settings[UO_input_tab_size].n);
        continue;
        }
      else
        {
        //fprintf(stderr, "%d] Text starts in col %d\n", line_count, ccol);
        }
      }

    line[line_len++] = ch;

      /* If we just hit an end of line OR we just hit end-of-comment... */
    if ((ch == '\n') || (remaining == 0))
      {
      line_count++;

        /* strip trailing tabs and spaces before the newline */
      if (ch == '\n')
        {
        line_len--;

        while ((line_len > 0) &&
               ((line[line_len - 1] == ' ') ||
                (line[line_len - 1] == '\t')))
          line_len--;

        line[line_len++] = ch;
        }

      line[line_len] = 0;

      if (line_count == 1)
        {
        /* this is the first line - add unchanged */

          /*TODO: need to support indent_with_tabs mode 1 */
        output_to_column(cmt_col, cpd.settings[UO_indent_with_tabs].b);
        add_text_len(line, line_len);
        }
      else
        {
        /* This is not the first line, so we need to indent to the
         * correct column.
         */
        ccol -= col_diff;

        if (ccol < cmt_col)
          ccol = cmt_col;

        if (line[0] == '\n')
          {
            /* Emtpy line - just a '\n' */
          if (cpd.settings[UO_cmt_star_cont].b)
            {
            output_to_column(cmt_col, cpd.settings[UO_indent_with_tabs].b);
            add_text((xtra == 1) ? " *" : "*");
            }

          add_char('\n');
          }
        else
          {
            /* If this doesn't start with a '*' or '|' */
          if ((line[0] != '*') && (line[0] != '|') && (line[0] != '#') &&
              (line[0] != '\\') && (line[0] != '+'))
            {
            output_to_column(cmt_col, cpd.settings[UO_indent_with_tabs].b);

            if (cpd.settings[UO_cmt_star_cont].b)
              add_text((xtra == 1) ? " * " : "*  ");
            else
              add_text("   ");

            output_to_column(ccol, cpd.settings[UO_indent_with_tabs].b);
            }
          else
            output_to_column(cmt_col + xtra, cpd.settings[UO_indent_with_tabs].b);

          add_text_len(line, line_len);
          }
        }

      line_len = 0;
      ccol     = 1;
      }
    }
  }

