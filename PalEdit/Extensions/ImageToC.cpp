/*
 * Copyright 2005, Axel Dörfler, axeld@pinc-software.de. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */


#include "PAddOn.h"

#include <TranslationUtils.h>
#include <MessageFilter.h>
#include <Looper.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Bitmap.h>
#include <String.h>
#include <Alert.h>

#include <stdio.h>
#include <string.h>


class ImageToC : public PAddOn
{
	public:
		ImageToC();
		virtual ~ImageToC();

		virtual void MessageReceived(BMessage* message);

		virtual void AttachedToDocument();
		virtual void DetachedFromDocument();

	private:
		void DumpBitmap(BBitmap* bitmap, const char *name);

		BMessageFilter* fImageFilter;
};


const char*
color_space_to_string(color_space space)
{
	#define SPACE(type) case type: return #type

	switch (space) {
		SPACE(B_GRAY1);
		SPACE(B_CMAP8);
		SPACE(B_GRAY8);

		SPACE(B_RGB15);
		SPACE(B_RGBA15);
		SPACE(B_RGB16);
		SPACE(B_RGB24);
		SPACE(B_RGB32);
		SPACE(B_RGBA32);

		SPACE(B_RGB15_BIG);
		SPACE(B_RGBA15_BIG);
		SPACE(B_RGB16_BIG);
		SPACE(B_RGB24_BIG);
		SPACE(B_RGB32_BIG);
		SPACE(B_RGBA32_BIG);

		default:
			return NULL;
	}

	#undef SPACE
}


bool is_image_data(BMessage *message)
{
	if (message->what == B_SIMPLE_DATA)
	{
		entry_ref ref;
		if (message->FindRef("refs", &ref) == B_OK)
		{
			BNode node(&ref);
			BNodeInfo info(&node);
			char type[B_ATTR_NAME_LENGTH];
			if (info.InitCheck() == B_OK
				&& info.GetType(type) == B_OK
				&& !strncmp("image/", type, 6))
				return true;
		}
		else
		{
			const char *type;
			int32 index = 0;
			while (message->FindString("be:types", index++, &type) == B_OK)
			{
				if (!strcmp(type, "x-be-bitmap"))
					return true;
			}
		}
	}
	else if (message->what == B_MIME_DATA)
	{
		// ToDo: we may want to support old-style drops as well
		puts("ImageToC: old-style drop not yet supported.");
	}

	return false;
}


//	#pragma mark -


ImageToC::ImageToC()
	: PAddOn("Image To C Source")
{
}


ImageToC::~ImageToC()
{
}


void ImageToC::DumpBitmap(BBitmap* bitmap, const char *name)
{
	if (bitmap == NULL || bitmap->InitCheck() != B_OK)
	{
		(new BAlert("image to c", "Could not open image", "Ok"))->Go(NULL);
		return;
	}

	PEditText text(this, "Insert Bitmap");
	BString baseName;

	const char *colorSpace = color_space_to_string(bitmap->ColorSpace());
	if (colorSpace == NULL)
	{
		text.Insert("<Unknown color space, sorry!>\n");
		return;
	}
	if (bitmap->BitsLength() > 1024 * 1024)
	{
		text.Insert("<Image too large, maximum allowed is 1 MB>\n");
		return;
	}

	// cut off the suffix	
	char *suffix = strrchr(name, '.');
	if (suffix)
		baseName.Append(name, suffix - name);
	else
		baseName.Append(name);

	baseName.Capitalize();
	baseName.RemoveAll(" ");
	baseName.RemoveAll("-");
	baseName.RemoveAll("+");
	baseName.Prepend("k");

	char line[1024];
#ifdef __MWERKS__
	sprintf(line,
		"const %sWidth = %ld;\n"
		"const %sHeight = %ld;\n"
		"const %sColorSpace = %s;\n"
		"const %sBytesPerRow = %ld;\n\n",
		baseName.String(), bitmap->Bounds().IntegerWidth() + 1, baseName.String(),
		bitmap->Bounds().IntegerHeight() + 1, baseName.String(), colorSpace,
		baseName.String(), bitmap->BytesPerRow());
#else
	snprintf(line, sizeof(line),
		"const %sWidth = %ld;\n"
		"const %sHeight = %ld;\n"
		"const %sColorSpace = %s;\n"
		"const %sBytesPerRow = %ld;\n\n",
		baseName.String(), bitmap->Bounds().IntegerWidth() + 1, baseName.String(),
		bitmap->Bounds().IntegerHeight() + 1, baseName.String(), colorSpace,
		baseName.String(), bitmap->BytesPerRow());
#endif

	text.Insert(line);

	int32 bytesPerPixel = bitmap->BytesPerRow() / (bitmap->Bounds().IntegerWidth() + 1);
	switch (bytesPerPixel)
	{
		case 1:
		case 2:
		case 4:
			// all okay
			break;
		default:
			bytesPerPixel = 1;
	}

	int32 size = bitmap->BitsLength() / bytesPerPixel;

#ifdef __MWERKS__
	sprintf(line, "const %sData[%ld] = {", baseName.String(), size);
#else
	snprintf(line, sizeof(line), "const %sData[%ld] = {",
		baseName.String(), size);
#endif
	text.Insert(line);

	line[0] = '\0';
	char *pos = line;

	switch (bytesPerPixel)
	{
		case 1:
		{
			const uint8 *data = (const uint8 *)bitmap->Bits();

			for (int32 i = 0; i < size; i++) {
				if ((i % 16) == 0)
				{
					strcat(pos, "\n\t");
					text.Insert(line);
					line[0] = '\0';
					pos = line;
				}

				pos += sprintf(pos, "0x%02x, ", data[i]);
			}
			break;
		}
		case 2:
		{
			const uint16 *data = (const uint16 *)bitmap->Bits();

			for (int32 i = 0; i < size; i++) {
				if ((i % 12) == 0)
				{
					strcat(pos, "\n\t");
					text.Insert(line);
					line[0] = '\0';
					pos = line;
				}

				pos += sprintf(pos, "0x%04x, ", data[i]);
			}
			break;
		}
		case 4:
		{
			const uint32 *data = (const uint32 *)bitmap->Bits();

			for (int32 i = 0; i < size; i++) {
				if ((i % 8) == 0)
				{
					strcat(pos, "\n\t");
					text.Insert(line);
					line[0] = '\0';
					pos = line;
				}

				pos += sprintf(pos, "0x%08lx, ", data[i]);
			}
			break;
		}
	}

	strcat(line, "\n};\n");
	text.Insert(line);
}


void ImageToC::MessageReceived(BMessage *message)
{
	if (message->what != B_SIMPLE_DATA)
		return BHandler::MessageReceived(message);

	int32 index = 0;
	entry_ref ref;
	while (message->FindRef("refs", index++, &ref) == B_OK)
	{
		// ToDo: this is a bit boring, since we always get a B_RGB32...
		BBitmap *bitmap = BTranslationUtils::GetBitmap(&ref);
		DumpBitmap(bitmap, ref.name);

		delete bitmap;
	}

	// reply to negotiation message

	if (index == 1)
	{
		BMessage reply(B_COPY_TARGET);
		reply.AddString("be:types", "image/x-be-bitmap");
		message->SendReply(&reply, message);

		const void *data;
		ssize_t size;
		if (message->FindData("image/x-be-bitmap", B_MIME_TYPE, &data, &size) == B_OK)
		{
			BMemoryIO stream(data, size);
			BBitmap *bitmap = BTranslationUtils::GetBitmap(&stream);
			DumpBitmap(bitmap, "clip");

			delete bitmap;
		}
	}
}


void ImageToC::AttachedToDocument()
{
	class ImageFilter : public BMessageFilter {
		public:
			ImageFilter(BHandler *target)
				: BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE),
				fTarget(target)
			{
			}

			filter_result Filter(BMessage *message, BHandler **_target)
			{
				if (is_image_data(message))
					*_target = fTarget;

				return B_DISPATCH_MESSAGE;
			}

		private:
			BHandler *fTarget;
	} *imageFilter = new ImageFilter(this);

	fImageFilter = imageFilter;
	Looper()->AddCommonFilter(imageFilter);
}


void ImageToC::DetachedFromDocument()
{
	fImageFilter->Looper()->RemoveCommonFilter(fImageFilter);
	delete fImageFilter;
}


//	#pragma mark -


PAddOn*
new_pe_add_on(void)
{
	return new ImageToC();
}

