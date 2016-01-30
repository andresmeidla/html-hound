var native = require('bindings')('html-hound');

var HtmlHound = {
  getElements: function(html, tagStart) {
    return native.getElements(html, tagStart);
  },
  getElement: function(html, tagStart) {
    return native.getElement(html, tagStart);
  },
  parseLink: function(html) {
    return native.parseLink(html);
  }
};

module.exports = HtmlHound;
