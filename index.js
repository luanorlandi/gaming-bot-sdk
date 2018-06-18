"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = {
  MAX_TURN: 200
};
'use strict';

var _constants = require('./constants');

var _constants2 = _interopRequireDefault(_constants);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var width = parseInt(readline(), 10);
var height = parseInt(readline(), 10);

for (var i = 0; i < height; i += 1) {
  var line = readline();
}

var inputs = readline().split(' ');
var sanityLossLonely = parseInt(inputs[0], 10);
var sanityLossGroup = parseInt(inputs[1], 10);
var wandererSpawnTime = parseInt(inputs[2], 10);
var wandererLifeTime = parseInt(inputs[3], 10);

printErr(_constants2.default);

// game loop
while (true) {
  var entityCount = parseInt(readline(), 10);
  for (var _i = 0; _i < entityCount; _i += 1) {
    var _inputs = readline().split(' ');
    var entityType = _inputs[0];
    var id = parseInt(_inputs[1], 10);
    var x = parseInt(_inputs[2], 10);
    var y = parseInt(_inputs[3], 10);
    var param0 = parseInt(_inputs[4], 10);
    var param1 = parseInt(_inputs[5], 10);
    var param2 = parseInt(_inputs[6], 10);
  }

  print('WAIT');
}
