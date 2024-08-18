function h(str) {
    if (str) {
        str = str.replace(/&/g, "&amp;");
        str = str.replace(/</g, "&lt;");
        str = str.replace(/>/g, "&gt;");
        return str;
    } else {
        return "";
    }
}

function hh(str) {
    if (str) {
        str = str.replace(/&/g, "&amp;");
        str = str.replace(/</g, "&lt;");
        str = str.replace(/>/g, "&gt;");
        str = str.replace(/\n/g, "<br />");
        return str;
    } else {
        return "";
    }
}