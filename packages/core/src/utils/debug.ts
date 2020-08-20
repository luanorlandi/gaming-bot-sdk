const debugMap = (map: Map<number, number>): string => {
  let linksString = '';

  const entries = map.entries();
  let next = entries.next();

  while (!next.done) {
    linksString = `${linksString}${next.value[0]} => ${next.value[1]}`;
    next = entries.next();

    if (!next.done) {
      linksString = `${linksString}, `;
    }
  }

  return linksString;
};

export { debugMap };

export default debugMap;
