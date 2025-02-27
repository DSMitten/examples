const ColorReset = '\x1b[0m';
const ColorRed = '\x1b[31m';

export function getLogLevel() {
    const logLevel = process.env.npm_config_loglevel;

    if (logLevel != null) {
        return logLevel;
    }

    return 'notice';
}

export function isVerboseLogging() {
    const logLevel = getLogLevel();
    return logLevel === 'verbose' || logLevel === 'silly';
}

export function isSillyLogging() {
    const logLevel = getLogLevel();
    return logLevel === 'silly';
}

export function logMessage(message) {
    console.log(message);
}

export function logWarning(message) {
    console.warn(message);
}

export function logError(err) {
    // If err is Error object with iterable output property, get the
    // output lines and log them.
    if (typeof err === 'object' && 'output' in err && Symbol.iterator in err.output) {
        for (const line of err.output) {
            console.error(ColorRed + line);
        }
    } else if (typeof err === 'string') {
        console.error(ColorRed + err);
    } else {
        console.error(err);
    }
}

export function logVerbose(message) {
    if (isVerboseLogging()) {
        console.log(message);
    }
}

export function logSilly(message) {
    if (isSillyLogging()) {
        console.log(message);
    }
}
