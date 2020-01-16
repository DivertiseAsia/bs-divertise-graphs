const path = require('path');
const outputDir = path.join(__dirname, 'build/');

const isProd = process.env.NODE_ENV === 'production';
const isMobile = process.env.MOBILE_BUILD || false;

let publicPath = isMobile ? '' : '/';
if (process.env.PUBLIC_PATH) {
  publicPath = process.env.PUBLIC_PATH;
}

try {
  versioning = JSON.stringify({
    VERSION: gitRevisionPlugin.version(),
    COMMITHASH: gitRevisionPlugin.commithash(),
    BRANCH: gitRevisionPlugin.branch(),
    MOBILE: isMobile,
  });
} catch (e) {
  versioning = JSON.stringify({
    VERSION: undefined,
    COMMITHASH: '000000',
    BRANCH: 'n/a',
    MOBILE: isMobile,
  })
}

console.log("webpack config is here");

module.exports = {
  entry: {
    'main': './lib/js/src/Index.bs.js'
  },
  output: {
    path: outputDir,
    publicPath,
    filename: '[name].js'.toLowerCase(),
    chunkFilename: '[name].chunk.js'.toLowerCase()
  },
  devServer: {
    compress: false,
    contentBase: outputDir,
    port: process.env.PORT || 8080,
    historyApiFallback: true
  }
};
