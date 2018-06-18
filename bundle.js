'use strict';

var MAX_TURN = "200";
var constants = {
	MAX_TURN: MAX_TURN
};

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

printErr(constants);

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
