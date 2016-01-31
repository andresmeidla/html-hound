var hound = require("../index");
var fs = require("fs");
var xml = fs.readFileSync(__dirname + '/' +'./test.xml', 'utf8');

var testData  = `
<div 1>
  <div 2>
    <div 3>
      <div 4>
        <div class="heading">
          <div 6>
            <div 7>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
`;

var utf8Data = 'ñø÷<div>ñø÷<div class="heading">ñø÷asfsaf<a href=\'http://blabla.ee\'>blabla</a>asgaghaszhsd<div>ʥʯʳ</div>ʥʯʳ</div>ʥʯʳ</div>ʥʯʳ'

var expect = require('chai').expect;
describe('xml-hound', function() {
  describe('getElements()', function () {
    it('should return 42 headings', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements(xml.toString(), tagStart);
      /*ret.forEach(function(item) {
        console.log(xml.toString().substring(item.start, item.start + item.len));
      });*/
      expect(ret.length).to.be.equal(42);
    });
    it('should return 1 heading', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements(testData.toString(), tagStart);
      expect(ret.length).to.be.equal(1);
    });
    it('should return 1 heading', function () {
      var tagStart = '<span class="frontComments dataLink" data-href="http://majandus24.postimees.ee/3471987/kohtla';
      var ret = hound.getElements(xml.toString(), tagStart);
      expect(ret.length).to.be.equal(1);
      expect(ret[0].start).to.equal(38648);
      expect(ret[0].len).to.equal(191);
    });
    it('should throw an exception', function () {
      var tagStart = 'div class="heading"';
      try {
        var ret = hound.getElements(xml.toString(), tagStart);
        throw new Error('this shouldve thrown');
      }
      catch(err) {
        console.log(err);
      }
    });
    it('should throw an exception', function () {
      var tagStart = '';
      try {
        var ret = hound.getElements(xml.toString(), tagStart);
        throw new Error('this shouldve thrown');
      }
      catch(err) {
        console.log(err);
      }
    });
    it('return empty array', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements('', tagStart);
      expect(ret.length).to.be.eql(0);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements(tagStart+tagStart+tagStart+tagStart+tagStart+tagStart+tagStart, tagStart);
      expect(ret.length).to.be.eql(0);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements(tagStart+'asfafjqnohquoqwh9 89qwe9qw989', tagStart);
      expect(ret.length).to.be.eql(0);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements(tagStart, tagStart);
      expect(ret.length).to.be.eql(0);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements('blabla', tagStart);
      expect(ret.length).to.be.eql(0);
    });
  });
  describe('getElement()', function () {
    it('should find element', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(testData.toString(), tagStart);
      expect(ret.start).to.equal(53);
      expect(ret.len).to.equal(110);
    });
    it('should find element from utf8 string', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(utf8Data, tagStart);
      expect(ret.start).to.equal(11);
      expect(ret.len).to.equal(102);
    });
    it('should find element', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(xml, tagStart);
      expect(ret.start).to.equal(141000);
      expect(ret.len).to.equal(221);
    });
    it('should throw an exception', function () {
      var tagStart = 'div class="heading"';
      try {
        var ret = hound.getElement(xml.toString(), tagStart);
        throw new Error('this shouldve thrown');
      }
      catch(err) {
        console.log(err);
      }
    });
    it('should throw an exception', function () {
      var tagStart = '';
      try {
        var ret = hound.getElement(xml.toString(), tagStart);
        throw new Error('this shouldve thrown');
      }
      catch(err) {
        console.log(err);
      }
    });
    it('should return no result', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement('', tagStart);
      expect(ret.start).to.equal(-1);
      expect(ret.len).to.equal(-1);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(tagStart+tagStart+tagStart+tagStart+tagStart+tagStart+tagStart, tagStart);
      expect(ret.start).to.equal(0);
      expect(ret.len).to.equal(-1);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(tagStart+'asfafjqnohquoqwh9 89qwe9qw989', tagStart);
      expect(ret.start).to.equal(0);
      expect(ret.len).to.equal(-1);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(tagStart, tagStart);
      expect(ret.start).to.equal(0);
      expect(ret.len).to.equal(-1);
    });
    it('should not crash', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement('blabla', tagStart);
      expect(ret.start).to.equal(-1);
      expect(ret.len).to.equal(-1);
    });
  });
  describe('parseLink()', function () {
    it('should parse link', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(xml.toString(), tagStart);
      expect(ret.start).to.equal(141000);
      expect(ret.len).to.equal(221);
      var div = xml.substr(ret.start, ret.len);
      ret = hound.parseLink(div);
      console.log(ret);
      expect(ret).to.exist;
    });
    it('should parse link', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElement(utf8Data.toString(), tagStart);
      expect(ret.start).to.equal(11);
      expect(ret.len).to.equal(102);
      var div = utf8Data.substr(ret.start, ret.len);
      ret = hound.parseLink(div);
      console.log(ret);
      expect(ret).to.exist;
    });
    it('should not find a link', function () {
      var ret = hound.parseLink(testData);
      expect(ret).to.not.exist;
    });
  });
});
