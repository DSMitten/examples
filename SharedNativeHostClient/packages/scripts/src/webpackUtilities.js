// External imports
import webpack from 'webpack';

// Local imports
import { logError, logVerbose, logWarning } from './logUtilities.js';

export function processWebpackOutput(err, stats) {
    if (err) {
        logError(err.stack || err);

        if (err.details) {
            logError(err.details);
        }

        return -1;
    }
}

export async function callWebpack(webpackConfig, packageDir) {
    logVerbose(`running webpack in ${packageDir}`);

    return new Promise((resolve, reject) => {
        webpack(webpackConfig, (err, stats) => {
            if (err) {
                logError(err.stack || err);
                if (err.details) {
                    logError(err.details);
                }
                reject('webpack failed');
                return;
            }

            if (stats.hasErrors()) {
                logError(stats.toString());
                reject('webpack failed');
            } else if (stats.hasWarnings()) {
                logWarning(stats.toString());
                reject('webpack warned');
            }

            logVerbose(stats.toString());
            resolve();
        });
    });
}
