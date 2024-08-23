

let field_guide;
let watch_id;
function start_field_guide(guide) {
    field_guide = guide;
    window.console.log("start!!");
    let points = [];
    points.push([-10, 0, 0]);
    points.push([0, 10, 0]);
    points.push([10, 0, 0]);
    // field_guide.updateLine(points);

    let update_btn = document.querySelector("#update");
    update_btn.onclick = clickUpdate;

    let add_btn = document.querySelector("#addIdoKeido");
    add_btn.onclick = addIdoKeido;

    let btn_to_camera = document.querySelector("#btn_to_camera");
    btn_to_camera.onclick = clickToCamera;

    let btn_get_info = document.querySelector("#btn_get_info");
    btn_get_info.onclick = clickGetInfo;

    if (navigator.geolocation) {
        console.log("onload watchPosition");
        watch_id = navigator.geolocation.watchPosition(
            (pos) => { handlePositon(pos);}
            , function(err) {
                console.log("エラー：" + err.code + ":" + err.message);
            }
            ,{
                timeout:5000,
                maximumAge: 0,
                enableHighAccuracy: true
            }
        );
    }
}

function rad(degrees) {
    return degrees * Math.PI / 180;
}

const EQUATORIAL_RADIUS = 6378137;


function rad(angle) {
    return angle * Math.PI / 180;
}

function distance(lat1, lon1, lat2, lon2) {
    let a = Math.sin(rad(lat1)) * Math.sin(rad(lat2))
            + Math.cos(rad(lat1)) * Math.cos(rad(lat2))
              * Math.cos(rad(lon2) - rad(lon1));
    let distance = EQUATORIAL_RADIUS * Math.acos(a);
    return distance;
}

function getResult() {
    let result = document.querySelector("#result");
    let jsondata = JSON.parse(result.value);
    return jsondata;

}
function setResult(jsondata) {
    let result = document.querySelector("#result");
    result.value = JSON.stringify(jsondata, null, 3);
}

function clickToCamera() {
    console.log("clickUpdate");
    let to_camera = document.querySelector("#to_camera");
    let n = to_camera.value.split(",");
    let x = Number(n[0]);
    let y = Number(n[1]);
    let z = Number(n[2]);
    console.log("clickUpdate x=" + x + ", y=" + y + ", z=" + z);
    field_guide.updateToCamera(x, y, z);
}

function clickGetInfo() {
    console.log("clickGetInfo");
    let control_info = document.querySelector("#control_info");
    // let info = {"カメラ位置": [0, 0, 0]};
    let info = field_guide.getInfo();
    control_info.value = JSON.stringify(info, null, 3);
    console.log("clickGetInfo <<<< end");
}

function clickUpdate() {
    console.log("clickUpdate");
    // let result = document.querySelector("#result");
    let jsondata = getResult();
    console.log(JSON.stringify(jsondata, null, 3));

    let keiro = jsondata["経路"];
    let points = [];

    if (keiro.length <= 1) {
        return;
    }
    let s_ido = Number(keiro[0]["緯度"]);
    let s_keido = Number(keiro[0]["経度"]);
    
    for (let i = 0; i < keiro.length; i++) {
        let ido = Number(keiro[i]["緯度"]);
        let keido = Number(keiro[i]["経度"]);
        let x = distance(ido, keido, ido, s_keido)
        if (keido < s_keido) {
            x = -x;
        }
        let y = distance(ido, keido, s_ido, keido)
        if (ido > s_ido) {
            y = -y;
        }
        console.log("緯度：" + y + " 経度：" + x);
        points.push([x, -y, 0]);
    }
    field_guide.updateLine(points);
}


function addIdoKeido() {
    console.log("addIdoKeido");
    let idokeido = document.querySelector("#idokeido");
    let n = idokeido.value.split(",");
    console.log("addIdoKeido" + n);


    let jsondata = getResult();
    jsondata["経路"].push({"緯度": n[0], "経度":n[1]})
    setResult(jsondata);
}

function handlePositon(pos) {
    //var result = document.querySelector("#result");
    var ido = pos.coords.latitude;
    var keido = pos.coords.longitude;
    //var heading = pos.coords.heading;
    //msg = `${count}: 緯度（${ido}）経度（${keido}）方角（${heading}）`;
    let jsondata = getResult();
    let keiro = jsondata["経路"];
    if (keiro.length > 1) {
        let last_ido = keiro[keiro.length - 1]["緯度"];
        let last_keido = keiro[keiro.length - 1]["経度"];
        console.log("緯度：" + ido + "|" + last_ido);
        console.log("経度：" + keido + "|" + last_keido);
        let d = distance(Number(ido), Number(keido), Number(last_ido), Number(last_keido));
        console.log("距離" + d);
        if (d < 1) {
            return;
        }
    }
    jsondata["経路"].push({"緯度": Number(ido), "経度":Number(keido)})
    setResult(jsondata);
    clickUpdate();
}

