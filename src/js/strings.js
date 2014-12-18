function toUTF8Array(str, finalLength) {
    var utf8 = unescape(encodeURIComponent(str));
    var arr = [];
    for (var i = 0; i < utf8.length && i < finalLength - 1; i++) {
        arr.push(utf8.charCodeAt(i));
    }
    for (; i < finalLength; i++) {
        arr.push(0);
    }
    return arr;
}
