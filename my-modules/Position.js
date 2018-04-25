const distance = (a, b) => {
  const x = a.x - b.x;
  const y = a.y - b.y;

  return Math.sqrt((x ** 2) + (y ** 2));
};

export default distance;
