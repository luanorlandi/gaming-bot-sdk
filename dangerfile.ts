// eslint-disable-next-line import/no-extraneous-dependencies
import { danger, warn } from 'danger';
import { codeCoverage } from 'danger-plugin-code-coverage';

codeCoverage([{
  title: '# Coverage',
  ignoreCoveragePattern: ['.test.', '.snap'],
  coverageFilesPath: 'packages/core/coverage/coverage-final.json',
}]);

// No PR is too small to include a description of why you made a change
if (danger?.github?.pr?.body?.length < 10) {
  warn('Please include a description of your PR changes.');
}
