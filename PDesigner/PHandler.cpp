#include "PHandler.h"

PHandler::PHandler(void)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::PHandler(BMessage *msg)
	:	PObject(msg)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::PHandler(const char *name)
	:	PObject(name)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::PHandler(const PHandler &from)
	:	PObject(from)
{
	fType = "PHandler";
	fFriendlyType = "Handler";
	AddInterface("PHandler");
}


PHandler::~PHandler(void)
{
}


BArchivable *
PHandler::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PHandler"))
		return new PHandler(data);

	return NULL;
}


PObject *
PHandler::Create(void)
{
	return new PHandler();
}


PObject *
PHandler::Duplicate(void) const
{
	return new PHandler(*this);
}


void
PHandler::SetMsgHandler(const int32 &constant, MethodFunction handler)
{
	fMsgHandlerMap[constant] = handler;
}


MethodFunction
PHandler::GetMsgHandler(const int32 &constant)
{
	MsgHandlerMap::iterator i = fMsgHandlerMap.find(constant);
	return (i == fMsgHandlerMap.end()) ? NULL : i->second;
}


void
PHandler::RemoveMsgHandler(const int32 &constant)
{
	fMsgHandlerMap.erase(constant);
}


status_t
PHandler::RunMessageHandler(const int32 &constant, PArgList &args)
{
	MsgHandlerMap::iterator i = fMsgHandlerMap.find(constant);
	if (i == fMsgHandlerMap.end())
		return B_NAME_NOT_FOUND;
	
	PArgList out;
	return i->second(this, &args, &out);
}


void
PHandler::ConvertMsgToArgs(BMessage &in, PArgList &out)
{
	char *fieldName;
	uint32 fieldType;
	int32 fieldCount;
	
	empty_parglist(&out);
	
	int32 i = 0;
	while (in.GetInfo(B_ANY_TYPE, i, &fieldName, &fieldType, &fieldCount) == B_OK)
	{
		for (int32 j = 0; j < fieldCount; j++)
		{
			void *ptr = NULL;
			ssize_t size = 0;
			in.FindData(fieldName, fieldType, j, (const void**)&ptr, &size);
			
			PArgType pargType;
			switch (fieldType)
			{
				case B_INT8_TYPE:
					pargType = PARG_INT8;
					break;
				case B_INT16_TYPE:
					pargType = PARG_INT16;
					break;
				case B_INT32_TYPE:
					pargType = PARG_INT32;
					break;
				case B_INT64_TYPE:
					pargType = PARG_INT64;
					break;
				case B_FLOAT_TYPE:
					pargType = PARG_FLOAT;
					break;
				case B_DOUBLE_TYPE:
					pargType = PARG_DOUBLE;
					break;
				case B_BOOL_TYPE:
					pargType = PARG_BOOL;
					break;
				case B_CHAR_TYPE:
					pargType = PARG_CHAR;
					break;
				case B_STRING_TYPE:
					pargType = PARG_STRING;
					break;
				case B_RECT_TYPE:
					pargType = PARG_RECT;
					break;
				case B_POINT_TYPE:
					pargType = PARG_POINT;
					break;
/*				case B_RGB_32_BIT_TYPE:
					pargType = PARG_COLOR;
					break;
				case B_RGB_COLOR_TYPE:
					pargType = PARG_COLOR;
					break;
*/				case B_POINTER_TYPE:
					pargType = PARG_POINTER;
					break;
				default:
					pargType = PARG_RAW;
					break;
			}
			add_parg(&out, fieldName, ptr, size, pargType);
		}
		
		i++;
	}
	add_parg_int32(&out, "what", in.what);
}


void
PHandler::ConvertArgsToMsg(PArgList &in, BMessage &out)
{
	PArgListItem *item = get_parg_first(&in);
	out.MakeEmpty();
	
	while  (item)
	{
		switch (item->type)
		{
			case PARG_INT8:
			{
				out.AddInt8(item->name, *((int8*)item->data));
				break;
			}
			case PARG_INT16:
			{
				out.AddInt16(item->name, *((int16*)item->data));
				break;
			}
			case PARG_INT32:
			{
				out.AddInt32(item->name, *((int32*)item->data));
				break;
			}
			case PARG_INT64:
			{
				out.AddInt64(item->name, *((int64*)item->data));
				break;
			}
			case PARG_FLOAT:
			{
				out.AddFloat(item->name, *((float*)item->data));
				break;
			}
			case PARG_DOUBLE:
			{
				out.AddDouble(item->name, *((double*)item->data));
				break;
			}
			case PARG_BOOL:
			{
				out.AddBool(item->name, *((bool*)item->data));
				break;
			}
			case PARG_CHAR:
			{
				out.AddInt8(item->name, *((int8*)item->data));
				break;
			}
			case PARG_STRING:
			{
				out.AddString(item->name, (char*)item->data);
				break;
			}
			case PARG_RECT:
			{
				out.AddRect(item->name, *((BRect*)item->data));
				break;
			}
			case PARG_POINT:
			{
				out.AddPoint(item->name, *((BPoint*)item->data));
				break;
			}
//			case PARG_COLOR:
//			{
//				out.AddInt8(item->name, *((int8*)item->data));
//				break;
//			}
			case PARG_POINTER:
			{
				out.AddPointer(item->name, item->data);
				break;
			}
			default:
			{
			//	out.AddInt8(item->name, *((int8*)item->data));
				break;
			}
		}
	}
}


	
status_t
PHandler::SendMessage(BMessage *msg)
{
	// This message is meant to be implemented by child classes
	return B_OK;
}

