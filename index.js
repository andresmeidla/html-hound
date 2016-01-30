var native = require('bindings')('html-hound');

var HtmlHound = {
  getElements: function(html, tagStart) {
    return JSON.parse(native.getElements(html, tagStart));
  },
  getElement: function(html, tagStart) {
    return JSON.parse(native.getElement(html, tagStart));
  }
};

module.exports = HtmlHound;
