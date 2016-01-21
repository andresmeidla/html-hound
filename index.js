var native = require('bindings')('xml-hound');

var XmlHound = {
  getElements: function(xml, tagStart) {
    return JSON.parse(native.getElements(xml, tagStart));
  },
  getElement: function(xml, tagStart) {
    return JSON.parse(native.getElement(xml, tagStart));
  }
};

module.exports = XmlHound;
