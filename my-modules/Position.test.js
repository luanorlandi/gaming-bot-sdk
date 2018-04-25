import distance from './Position';

test('distance of (0, 0) to (3, 4)', () => {
  const a = { x: 0, y: 0 };
  const b = { x: 3, y: 4 };
  expect(distance(a, b)).toBe(5);
});
