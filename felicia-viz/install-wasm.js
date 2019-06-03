const fs = require('fs');
const path = require('path');
const rmdirRecursive = require('rmdir-recursive');
const tar = require('tar-fs');

const TAR_PATH = path.resolve('../bazel-bin/felicia/wasm/felicia_wasm.js.tar');
const TARGET_PATH = path.resolve('./wasm');

function callback(err) {
  if (err) throw err;
  fs.createReadStream(TAR_PATH)
    .pipe(tar.extract(TARGET_PATH))
    .on('finish', () => {
      fs.readdir(TARGET_PATH, (err, files) => {
        if (err) throw err;
        for (const file of files) {
          if (path.extname(file) === '.js') {
            fs.appendFileSync(path.resolve(TARGET_PATH, file), 'export default Module;');
          }
        }
        console.log('Done!');
      });
    });
}

fs.access(TARGET_PATH, fs.constants.F_OK, err => {
  if (err) {
    fs.mkdir(TARGET_PATH, {}, callback);
  } else {
    rmdirRecursive(TARGET_PATH, callback);
  }
});
