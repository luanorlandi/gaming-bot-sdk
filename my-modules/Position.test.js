import Position from './Position.ts';

test('distance of (0, 0) to (3, 4)', () => {
  const positionA = new Position(0, 0);
  const positionB = new Position(3, 4);
  const distance = positionA.distanceTo(positionB);
  expect(distance).toEqual(5);
});
