var logger;
var status_updater;

var templog_req = {
    GET_VERSION  : {'num': 0, 'dir': 'in', 'len': 4},
    SET_TIME     : {'num': 1, 'dir': 'out'},
    GET_TIME     : {'num': 2, 'dir': 'in', 'len': 4},
    SET_INTERVAL : {'num': 3, 'dir': 'out'},
    GET_INTERVAL : {'num': 4, 'dir': 'in', 'len': 2},
    SET_MODE     : {'num': 5, 'dir': 'out'},
    GET_MODE     : {'num': 6, 'dir': 'in', 'len': 1},
    ERASE_DATA   : {'num': 7, 'dir': 'out'},
    GET_COUNT    : {'num': 8, 'dir': 'in', 'len': 8},
    GET_TEMP     : {'num': 10, 'dir': 'in', 'len': 2},
    GET_DATA     : {'num': 12, 'dir': 'in', 'len': 0}
};


function do_req(reqinfo, cb) {
    if (Object.isString(reqinfo)) {
        reqinfo = {'req': reqinfo};
    }

    var req = Object.merge(Object.clone(reqinfo), templog_req[reqinfo.req]);
    var wIndex = 0;             // interface 0
    var wValue = req.value || 0;
    var bRequest = req.num;

    chrome.usb.controlTransfer(logger,
                               {
                                   'direction': req.dir,
                                   'recipient': 'interface',
                                   'requestType': 'vendor',
                                   'request': bRequest,
                                   'value': wValue,
                                   'index': wIndex,
                                   'length': req.len,
                                   'data': req.data
                               }, function(ti) {
                                   if (ti.resultCode != 0) {
                                       connection_error();
                                   } else if (cb) {
                                       cb(ti);
                                   }
                               });
}

function get_version(cb) {
    do_req('GET_VERSION', function(ti) {
        var dv = new DataView(ti.data);
        var version = dv.getUint32(0, true);

        cb(version);
    });
}

function set_time(time, cb) {
    var buf = new ArrayBuffer(4);
    var data = new DataView(buf);
    data.setUint32(0, time.valueOf() / 1000, true); // time in seconds
    do_req({'req': 'SET_TIME', 'data': buf}, cb);
}

function get_time(cb) {
    do_req('GET_TIME', function(ti) {
        var dv = new DataView(ti.data);
        var time = dv.getUint32(0, true);

        cb(Date.create(time * 1000));
    });
}

function set_interval(interval, cb) {
    do_req({'req': 'SET_INTERVAL', 'value': interval}, cb);
}

function get_interval(cb) {
    do_req('GET_INTERVAL', function(ti) {
        var dv = new DataView(ti.data);
        var interval = dv.getUint16(0, true);

        cb(interval);
    });
}

function set_mode(mode, cb) {
    do_req({'req': 'SET_MODE', 'value': mode}, cb);
}

function get_mode(cb) {
    do_req('GET_MODE', function(ti) {
        var dv = new DataView(ti.data);
        var mode = dv.getUint8(0, true);

        cb(mode);
    });
}

function get_count(cb) {
    do_req('GET_COUNT', function(ti) {
        var dv = new DataView(ti.data);
        var total = dv.getUint32(0, true);
        var used = dv.getUint32(4, true);

        cb(total, used);
    });
}

function connection_error() {
    if (status_updater) {
        window.clearInterval(status_updater);
        status_updater = null;
    }
    if (logger) {
        chrome.usb.closeDevice(logger);
        logger = null;
    }

    $('#notice-no-device').removeClass('hidden');
    $('#have-device-content').addClass('hidden');
}


function onLoggerFound(handles) {
    var success = handles.length == 1;

    if (success) {
        logger = handles[0];
        get_version(function(version) {
            $('#notice-no-device').addClass('hidden');
            $('#have-device-content').removeClass('hidden');
            $('#firmware-version').text(version);
        });

        update_status();
        status_updater = window.setInterval(update_status, 1000);
    } else {
        handles.each(function(h) {
            chrome.usb.closeDevice(h);
        });
        $('#notice-no-device').removeClass('hidden');
    }
}


function check_for_devices() {
    chrome.usb.findDevices({"vendorId": 0x2323, "productId": 0x0101}, onLoggerFound);
}

function sync_time() {
    set_time(Date.create(), function(ti) {
        update_status();
    });
}

function update_status() {
    get_time(function(time) {
        $('#current-time').text(time.format("{dd}.{MM}.{yyyy} {HH}:{mm}:{ss}"));
    });
    get_interval(function(interval) {
        $('#current-interval').text(interval);
    });
    get_count(function(total, used) {
        $('#total-flash').text(total);
        $('#used-flash').text(used);
    });
    get_mode(function(mode) {
        if (mode == 0) {
            $('#current-mode').text("stopped");
        } else {
            $('#current-mode').text("running");
        }
    });
}

function onLoad() {
    $('#refresh-button').click(check_for_devices);
    $('#sync-time-button').click(sync_time);
    check_for_devices();
}

onLoad();
