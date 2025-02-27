import {
  makeWebView2MessageChannel,
  startInteropSystem,
  logToConsole,
} from "@microsoft/wv2-interop";

import { getOSVersionInformation } from "@clientapp/clientapis";

async function startNativeHostInterop() {
  const webView2MessageChannel = makeWebView2MessageChannel();

  await startInteropSystem({
    channel: webView2MessageChannel,
    logger: logToConsole,
    verbTimeout: 1000000,
  });
}

function getButton(name: string): HTMLButtonElement {
  return document.getElementById(name) as HTMLButtonElement;
}

function getInput(name: string): HTMLInputElement {
  return document.getElementById(name) as HTMLInputElement;
}

async function getOSVersionButtonClicked() {
  try {
    if (getOSVersionInformation) {
      const osversion = getOSVersionInformation();
      await osversion;
      getInput("osDescriptionText").value = (await osversion).description;
      getInput("osVersionText").value = (await osversion).version;
    }
  } catch (err) {
    console.log(`Error: ${err}`);
  }
}

function initializeControls() {
  getButton("getOSVersionButton").onclick = getOSVersionButtonClicked;
}

async function initialize() {
  initializeControls();
  await startNativeHostInterop();
}

// call initialize and write any errors to the console
initialize().catch((error) => {
  console.error(error);
});
