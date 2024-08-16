function resizefull(canvas) {
    var displayWidth = window.innerWidth;
    var displayHeight = window.innerHeight;
    if (canvas.width != displayWidth) {
        canvas.width = displayWidth;
    }
    if (canvas.height != displayHeight) {
        canvas.height = displayHeight;
    }
}

var pointList = [];

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

function addPoint(e) {
    var point_list = document.querySelector("#point_list");
    console.log('clientX: ' + e.clientX);
    console.log('clientY: ' + e.clientY);
    console.log('pageX: ' + e.pageX);
    console.log('pageY: ' + e.pageY);
    var last = pointList.length;
    pointList[last] = {
        "x" : e.clientX,
        "y" : e.clientY,
    }
    point_list.value = JSON.stringify(pointList, null, 3);
    point_list.style = "opacity: 0.2;";
}
function length(x, y)  {
    return Math.sqrt( x*x + y*y);
}
var mouseDown = false;
function handleClick(e) {
    console.log("click");
    addPoint(e);
    mouseDown = false;
}
function handleDown(e) {
    console.log("down");
    addPoint(e);
    mouseDown = true;
}
function handleMove(e) {
    console.log(e.button);
    if (mouseDown) {
        addPoint(e);
    }
}
function loadText() {
    var point_list = document.querySelector("#point_list");
 
}
function hello() {
    console.log("111");
    var canvas = document.querySelector("#can");
    resizefull(canvas);
    var ctx = canvas.getContext("2d");
    ctx.fillStyle = "rgba(" + [0, 0, 255, 0.5] + ")";
    ctx.fillRect(0, 0, 150, 100);

    var point_txt = document.querySelector("#point_list");
    canvas.addEventListener('click', e => handleClick(e));
    canvas.addEventListener('mousedown', e => handleDown(e));
    canvas.addEventListener('mousemove', e => handleMove(e));
}
