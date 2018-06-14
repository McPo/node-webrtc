/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "fileaudiodevicefactory.h"

#include "common.h"
#include "mediastream.h"
#include "create-offer-observer.h"
#include "webrtc/modules/audio_device/dummy/file_audio_device_factory.h"

#include <iostream>

using node_webrtc::FileAudioDeviceFactory;
using v8::External;
using v8::Function;
using v8::FunctionTemplate;
using v8::Handle;
using v8::Integer;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Uint32;
using v8::Value;

Nan::Persistent<Function> FileAudioDeviceFactory::constructor;

// FileAudioDeviceFactory constructor/destructor
/*FileAudioDeviceFactory::FileAudioDeviceFactory() {
}

FileAudioDeviceFactory::~FileAudioDeviceFactory() {
  TRACE_CALL;
  TRACE_END;
}*/

// NodeJS Wrapping
NAN_METHOD(FileAudioDeviceFactory::New) {
  TRACE_CALL;

  if (!info.IsConstructCall()) {
    return Nan::ThrowTypeError("Use the new operator to construct the FileAudioDeviceFactory.");
  }

  FileAudioDeviceFactory* obj = new FileAudioDeviceFactory();
  obj->Wrap(info.This());

  TRACE_END;
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(FileAudioDeviceFactory::SetFilenamesToUse) {
  TRACE_CALL;
  v8::String::Utf8Value inputArg(info[0]->ToString());
  std::string input = std::string(*inputArg);

  v8::String::Utf8Value outputArg(info[1]->ToString());
  std::string output = std::string(*outputArg);

  webrtc::FileAudioDeviceFactory::SetFilenamesToUse(input.c_str(), output.c_str());
  TRACE_END;
}

void FileAudioDeviceFactory::Init(Handle<Object> exports) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("FileAudioDeviceFactory").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "setFilenamesToUse", SetFilenamesToUse);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("FileAudioDeviceFactory").ToLocalChecked(), tpl->GetFunction());
}
