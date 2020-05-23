import resolve from '@rollup/plugin-node-resolve';
import babel from 'rollup-plugin-babel';
import json from '@rollup/plugin-json';
import typescript from 'rollup-plugin-typescript2';

import pkg from '../../package.json';

export default {
  input: 'src/index.ts',
  output: [
    { file: pkg.main, format: 'cjs', sourcemap: true },
    { file: pkg.module, format: 'esm', sourcemap: true },
  ],
  plugins: [
    json(),
    typescript({ useTsconfigDeclarationDir: true }),
    resolve(),
    babel({
      exclude: 'node_modules/**',
      plugins: [
        'transform-class-properties',
      ],
    }),
  ],
};
