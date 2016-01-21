{
  "targets": [
    {
      "target_name": "xml-hound",
      "sources": [ "src/wrap.cpp", "src/xmlhound.cpp" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
       ]
    }
  ]
}
