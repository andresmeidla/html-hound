var hound = require("../index");
var fs = require("fs");
var xml = fs.readFileSync(__dirname + '/' +'./test.xml');

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

var expect = require('chai').expect;
describe('xml-hound', function() {
  describe('getElements()', function () {
    it('should return 42 headings', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements(xml.toString(), tagStart);
      expect(ret.length).to.be.equal(42);
    });
    it('should return 1 heading', function () {
      var tagStart = '<div class="heading"';
      var ret = hound.getElements(testData.toString(), tagStart);
      expect(ret.length).to.be.equal(1);
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
});
