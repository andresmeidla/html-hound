{
  "targets": [
    {
      "target_name": "html-hound",
      "sources": [ "src/wrapper.cpp", "src/htmlhound.cpp" ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
       ]
    }
  ]
}
