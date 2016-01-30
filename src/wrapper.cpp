#include <nan.h>
#include <stdio.h>
#include <string.h>
#include <locale>
#include <codecvt>
#include "htmlhound.h"

std::u32string TO_32BIT(const std::string &s)
{
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(s);
}

NAN_METHOD(getElements) {
  if (info.Length() < 2) {
    Nan::ThrowError("Wrong number of arguments");
    return;
  }

  if(info[0]->IsString() == false || info[1]->IsString() == false) {
    Nan::ThrowTypeError("Wrong argument types");
    return;
  }

  std::u32string html = TO_32BIT(std::string(*v8::String::Utf8Value(info[0]->ToString())));
  std::u32string startTag = TO_32BIT(std::string(*v8::String::Utf8Value(info[1]->ToString())));
  std::vector<HtmlHound::Position> positions;
  bool ret = HtmlHound::GetElements(html, startTag, positions);
  if(!ret) {
    Nan::ThrowError((std::string("getElements failed with ") + std::to_string(ret)).c_str());
    return;
  }
  std::string json;
  for (size_t i = 0; i < positions.size(); i++) {
    json += "{\"start\": " + std::to_string(positions[i].start) + ", \"len\": " + std::to_string(positions[i].len) + "}";
    if(i + 1 < positions.size())
      json += ',';
  }
  json = "[" + json + "]";
  info.GetReturnValue().Set(Nan::New(json.c_str()).ToLocalChecked());
}

NAN_METHOD(getElement) {
  if (info.Length() < 2) {
    Nan::ThrowError("Wrong number of arguments");
    return;
  }

  if(info[0]->IsString() == false || info[1]->IsString() == false) {
    Nan::ThrowTypeError("Wrong argument types");
    return;
  }
  v8::String::Utf8Value htmlV8(info[0]);
  const char* bla = *htmlV8;
  std::u32string html = TO_32BIT(std::string(bla));
  v8::String::Utf8Value tagV8(info[1]);
  std::u32string startTag = TO_32BIT(std::string(*tagV8));
  HtmlHound::Position position;
  bool ret = HtmlHound::GetElement(html, startTag, position);
  if(!ret) {
    Nan::ThrowError((std::string("getElement failed with ") + std::to_string(ret)).c_str());
    return;
  }
  std::string json = std::string("{\"start\": ") + std::to_string(position.start) + ", \"len\": " + std::to_string(position.len) + "}";
  info.GetReturnValue().Set(Nan::New(json.c_str()).ToLocalChecked());
}

NAN_MODULE_INIT(init) {
  Nan::Set(target, Nan::New<v8::String>("getElements").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getElements)->GetFunction());
  Nan::Set(target, Nan::New<v8::String>("getElement").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getElement)->GetFunction());
}

NODE_MODULE(htmlhound, init)
