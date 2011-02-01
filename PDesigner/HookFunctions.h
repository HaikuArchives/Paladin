#ifndef HOOKFUNCTIONS_H
#define HOOKFUNCTIONS_H

#include "CInterface.h"

int32_t PWindowWindowActivated(void *pobject, PArgList *in, PArgList *out);
int32_t PWindowFrameMoved(void *pobject, PArgList *in, PArgList *out);
int32_t PWindowFrameResized(void *pobject, PArgList *in, PArgList *out);
int32_t PWindowDestructor(void *pobject, PArgList *in, PArgList *out);
int32_t PWindowQuitRequested(void *pobject, PArgList *in, PArgList *out);
int32_t PWindowMQuitRequested(void *pobject, PArgList *in, PArgList *out);
int32_t PViewFocusChanged(void *pobject, PArgList *in, PArgList *out);
int32_t PViewMouseDown(void *pobject, PArgList *in, PArgList *out);
int32_t	PViewHandleFloaterMsg(void *pobject, PArgList *in, PArgList *out);

#endif
