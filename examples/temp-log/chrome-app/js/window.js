var logger;

function onLoggerFound(devices) {
    var success = devices.length == 1;

    $('#notice-no-device').toggleClass('hidden', success);
}

function check_for_devices() {
    chrome.usb.getDevices({"vendorId": 0x2323, "productId": 0x0101}, onLoggerFound);
}

function onLoad() {
    $('#refresh-button').click(check_for_devices);
    check_for_devices();
}

onLoad();
