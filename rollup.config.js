import resolve from 'rollup-plugin-node-resolve';
import babel from 'rollup-plugin-babel';
import json from 'rollup-plugin-json';
import typescript from 'rollup-plugin-typescript';

export default {
  input: 'src/compete/contest/code-of-kutulu/index.js',
  output: {
    format: 'cjs',
  },
  plugins: [
    json(),
    resolve(),
    typescript(),
    babel({
      exclude: 'node_modules/**',
      plugins: [
        'external-helpers',
        'transform-class-properties',
      ],
    }),
  ],
};
