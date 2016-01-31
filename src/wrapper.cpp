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

std::string TO_8BIT(const std::u32string& str)
{
  return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().to_bytes(str);
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
  bool ret = HtmlHound::getElements(html, startTag, positions);
  if(!ret) {
    Nan::ThrowError("getElements failed with ");
    return;
  }

  // create array to be returned
  v8::Local<v8::Array> elements = Nan::New<v8::Array>();
  std::string json;
  for (size_t i = 0; i < positions.size(); i++) {
    v8::Local<v8::Object> obj = Nan::New<v8::Object>();
    obj->Set(Nan::New<v8::String>("start").ToLocalChecked(), Nan::New<v8::Int32>(positions[i].start));
    obj->Set(Nan::New<v8::String>("len").ToLocalChecked(), Nan::New<v8::Int32>(positions[i].len));
    if(positions[i].start > -1 && positions[i].len > -1 ) {
      obj->Set(Nan::New<v8::String>("text").ToLocalChecked(), Nan::New<v8::String>(TO_8BIT(html.substr(positions[i].start, positions[i].len)).c_str()).ToLocalChecked());
    }
    Nan::Set(elements, i, obj);
  }
  info.GetReturnValue().Set(elements);
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
  bool ret = HtmlHound::getElement(html, startTag, position);
  if(!ret) {
    Nan::ThrowError("getElement failed");
    return;
  }
  // create return object
  v8::Local<v8::Object> obj = Nan::New<v8::Object>();
  obj->Set(Nan::New<v8::String>("start").ToLocalChecked(), Nan::New<v8::Int32>(position.start));
  obj->Set(Nan::New<v8::String>("len").ToLocalChecked(), Nan::New<v8::Int32>(position.len));
  if(position.start > -1 && position.len > -1 ) {
    obj->Set(Nan::New<v8::String>("text").ToLocalChecked(), Nan::New<v8::String>(TO_8BIT(html.substr(position.start, position.len)).c_str()).ToLocalChecked());
  }
  info.GetReturnValue().Set(obj);
}

NAN_METHOD(parseLink) {
  if (info.Length() < 1) {
    Nan::ThrowError("Wrong number of arguments");
    return;
  }

  // create return Object
  v8::Local<v8::Object> retObj = Nan::New<v8::Object>();
  std::u32string html = TO_32BIT(std::string(*v8::String::Utf8Value(info[0]->ToString())));
  HtmlHound::Link link;
  if(HtmlHound::parseLink(html, link)) {
    retObj->Set(Nan::New<v8::String>("url").ToLocalChecked(), Nan::New<v8::String>(TO_8BIT(link.url).c_str()).ToLocalChecked());
    retObj->Set(Nan::New<v8::String>("text").ToLocalChecked(), Nan::New<v8::String>(TO_8BIT(link.text).c_str()).ToLocalChecked());
    info.GetReturnValue().Set(retObj);
  }
}

NAN_MODULE_INIT(init) {
  Nan::Set(target, Nan::New<v8::String>("getElements").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getElements)->GetFunction());
  Nan::Set(target, Nan::New<v8::String>("getElement").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getElement)->GetFunction());
  Nan::Set(target, Nan::New<v8::String>("parseLink").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(parseLink)->GetFunction());
}

NODE_MODULE(htmlhound, init)
