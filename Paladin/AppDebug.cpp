#include "AppDebug.h"

#include "Project.h"
#include "SourceFile.h"
#include <stdio.h>

void
DumpDependencies(Project *proj)
{
	printf("Dependencies for project %s:\n",proj->GetName());
	
	for (int32 i = 0; i < proj->CountGroups(); i++)
	{
		SourceGroup *group = proj->GroupAt(i);
		
		for (int32 j = 0; j < group->filelist.CountItems(); j++)
		{
			SourceFile *file = group->filelist.ItemAt(j);
			printf("%s: %s\n",file->GetPath().GetFileName(),
					file->GetDependencies());
		}
	}
}


void
DumpIncludes(Project *proj)
{
	printf("INCLUDES:\n---------\nLocal:\n");
	for (int32 i = 0; i < proj->CountLocalIncludes(); i++)
		printf("\t%s\n",proj->LocalIncludeAt(i));
	
	printf("System:\n");
	for (int32 i = 0; i < proj->CountSystemIncludes(); i++)
		printf("\t%s\n",proj->SystemIncludeAt(i));
}
