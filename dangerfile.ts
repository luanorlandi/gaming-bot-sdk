/* eslint-disable import/no-extraneous-dependencies */
import { danger, warn, schedule } from 'danger';
import { istanbulCoverage } from 'danger-plugin-istanbul-coverage';

schedule(
  istanbulCoverage({
    reportFileSet: 'createdOrModified',
    coveragePath: 'packages/core/coverage/coverage-summary.json',
  }),
);

// No PR is too small to include a description of why you made a change
if (danger?.github?.pr?.body?.length < 10) {
  warn('Please include a description of your PR changes.');
}
