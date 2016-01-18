#include <node.h>
#include <stdio.h>
#include "xmlhound.h"

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Exception;

void getElements(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  if (args.Length() < 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if(args[0]->IsString() == false || args[1]->IsString() == false) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong argument types")));
    return;
  }

  std::string xml = *v8::String::Utf8Value(args[0]->ToString());
  std::string startTag = *v8::String::Utf8Value(args[1]->ToString());
  std::vector<XmlHound::Position> tags;
  int ret = XmlHound::GetElements(xml, startTag, tags);
  if(ret != 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, (std::string("getTags failed with ") + std::to_string(ret)).c_str())));
    return;
  }
  v8::Handle<v8::Array> result = v8::Array::New(isolate, tags.size());
  for (size_t i = 0; i < tags.size(); i++)
    result->Set(i, String::NewFromUtf8(isolate, xml.substr(tags[i].start, tags[i].len).c_str()));
  args.GetReturnValue().Set(result);
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getElements", getElements);
}

NODE_MODULE(xmlhound, init)
