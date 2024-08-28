

let field_guide;
let watch_id;
let work_width = 16;
function start_field_guide(guide) {
    field_guide = guide;
    window.console.log("start!!");

    let update_btn = document.querySelector("#update");
    update_btn.onclick = clickUpdate;

    let add_btn = document.querySelector("#addIdoKeido");
    add_btn.onclick = clickAddIdoKeido;

    let btn_to_camera = document.querySelector("#btn_to_camera");
    btn_to_camera.onclick = clickToCamera;

    let btn_get_info = document.querySelector("#btn_get_info");
    btn_get_info.onclick = clickGetInfo;


    let init_guide = document.querySelector("#init_guide");
    init_guide.onclick = click_init_guide;
    let refline_start = document.querySelector("#refline_start");
    refline_start.onclick = click_refline_start;
    let refline_end = document.querySelector("#refline_end");
    refline_end.onclick = click_refline_end;
    let work_width_3200 = document.querySelector("#work_width_3200");
    work_width_3200.onclick = click_work_width_3200;
    let work_width_16000 = document.querySelector("#work_width_16000");
    work_width_16000.onclick = click_work_width_16000;

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

function click_init_guide() {
    let init_guide = document.querySelector("#init_guide");
    init_guide.blur();
    console.log("init_guide");
    let points = [];
    points.push([0, 0, 0]);
    field_guide.updateLine(points);
    let jsondata = {};
    jsondata["基準起点"] = {};
    jsondata["基準終点"] = {};
    jsondata["作業幅"] = work_width;
    jsondata["経路"] = [];
    setResult(jsondata);
    msg("経路情報と基準線情報を消しました。")
}
function click_refline_start() {
    console.log("基準起点");
    let jsondata = getResult();
    let keiro = jsondata["経路"];
    if (keiro.length > 0) {
        let last_ido = keiro[keiro.length - 1]["緯度"];
        let last_keido = keiro[keiro.length - 1]["経度"];
        jsondata["基準起点"] = {"緯度": Number(last_ido), "経度":Number(last_keido)};
    }
    setResult(jsondata);
    clickUpdate();
    msg("基準線の起点を設定しました。");
}
function click_refline_end() {
    console.log("基準終点");
    let jsondata = getResult();
    let keiro = jsondata["経路"];
    if (keiro.length > 0) {
        let last_ido = keiro[keiro.length - 1]["緯度"];
        let last_keido = keiro[keiro.length - 1]["経度"];
        jsondata["基準終点"] = {"緯度": Number(last_ido), "経度":Number(last_keido)};
    }
    setResult(jsondata);
    clickUpdate();
    msg("基準線の終点を設定しました。");
}
function click_work_width_3200() {
    let jsondata = getResult();
    work_width = 3.2;
    jsondata["作業幅"] = work_width;
    setResult(jsondata);
    clickUpdate();
    msg("作業幅の表示を3.2m（ウイングハローなど）に設定しました。");
}
function click_work_width_16000() {
    let jsondata = getResult();
    work_width = 16.0;
    jsondata["作業幅"] = work_width;
    setResult(jsondata);
    clickUpdate();
    msg("作業幅の表示を16m（BSA-651など）に設定しました。");
}






function rad(degrees) {
    return degrees * Math.PI / 180;
}

function unitVec(vec) {
    let r = Math.sqrt(vec.x * vec.x + vec.y * vec.y);
    if (r == 0) {
        return {x:0, y:0, r:1};
    }
    return {x: (vec.x / r), y: (vec.y / r), r : r};
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
function getIdoKeidoPoint(s_ido, s_keido, ido, keido) {
    let n_ido = Number(ido);
    let n_keido = Number(keido);
    let x = distance(n_ido, n_keido, n_ido, s_keido);
    if (keido < s_keido) {
        x = -x;
    }
    let y = distance(ido, keido, s_ido, keido)
    if (ido > s_ido) {
        y = -y;
    }
    return [x, y];
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
    // 基準起点の処理。
    let rls = jsondata["基準起点"];
    console.log("基準起点の処理" + rls);
    let rls_x = 0;
    let rls_y = 0;
    if (rls) {
        console.log("基準起点の処理２" + rls);
        let ido = rls["緯度"];
        let keido = rls["経度"];
        if (ido && keido) {
            let ikp = getIdoKeidoPoint(s_ido, s_keido, ido, keido);
            rls_x = ikp[0];
            rls_y = ikp[1];
            field_guide.setApproach(1, rls_x, -rls_y, -3, 5)
        }
    }
    // 基準終点の処理。
    let rle = jsondata["基準終点"];
    console.log("基準終点の処理" + rls);
    let rle_x = 0;
    let rle_y = 0;
    if (rle) {
        let ido = rle["緯度"];
        let keido = rle["経度"];
        if (ido && keido) {
            let ikp = getIdoKeidoPoint(s_ido, s_keido, ido, keido);
            rle_x = ikp[0];
            rle_y = ikp[1];
            field_guide.setApproach(2, rle_x, -rle_y, -3, 5)
        }
    }

    let fx;
    let fy;
    for (let i = 0; i < keiro.length; i++) {
        let ido = Number(keiro[i]["緯度"]);
        let keido = Number(keiro[i]["経度"]);
        let ikp = getIdoKeidoPoint(s_ido, s_keido, ido, keido);
        let x = ikp[0];
        let y = ikp[1];
        console.log("X：" + x + " Y：" + (-y));
        if (i > 0) {
            let ww = work_width / 2;
            let vec = {};
            vec.x = fx - x;
            vec.y = fy - y;
            vec = unitVec(vec);
            let tx = x + vec.x;
            let ty = y + vec.y;
            points.push([tx, -ty, 0]);
            let ltx = x + (-vec.y) * ww;
            let lty = y + (vec.x) * ww;
            let rtx = x + (vec.y) * ww;
            let rty = y + (-vec.x) * ww;
            points.push([ltx, -lty, 0]);
            points.push([rtx, -rty, 0]);
            points.push([tx, -ty, 0]);
        } else {
            points.push([x, -y, 0]);
        }
        fx = x;
        fy = y;
    }
    field_guide.updateLine(points);
    // 直交点の処理
    if (rls) {
        if (rle) {
            let rvec = unitVec({ x:(rls_x - rle_x), y:(rls_y - rle_y)});
            console.log("基準線" + rvec.r + ",x=" + rvec.x + ",y=" +  rvec.y);
            // 内積
            //let dvec =  unitVec({ x:(fx - rle_x), y:(rls_y - rle_y)});
            //console.log()
            let r = rvec.x * (fx - rle_x) + rvec.y * (fy - rle_y);
            maru_x = rle_x + r * rvec.x;
            maru_y = rle_y + r * rvec.y;
            console.log("距離" + r);
            field_guide.setApproach(3, maru_x, -maru_y, -2, 5);
            let nvec = unitVec({ x:(fx - maru_x), y:(fy - maru_y)});
            next_r = Math.round(nvec.r / work_width) * work_width;
            console.log("四捨五入" + next_r);
            field_guide.setApproach(4, maru_x + next_r * nvec.x, -(maru_y + next_r * nvec.y), -2, 6);
            next_r += work_width;
            console.log("切り上げ" + next_r);
            field_guide.setApproach(5, maru_x + next_r * nvec.x, -(maru_y + next_r * nvec.y), -2, 3);
        }
    }
}


function clickAddIdoKeido() {
    console.log("addIdoKeido");
    let idokeido = document.querySelector("#idokeido");
    let n = idokeido.value.split(",");
    console.log("addIdoKeido" + n);
    addIdoKeido(n[0], n[1]);
    clickUpdate();
}

function addIdoKeido(ido, keido) {
    console.log("addIdoKeido");
    //var heading = pos.coords.heading;
    //msg = `${count}: 緯度（${ido}）経度（${keido}）方角（${heading}）`;
    let jsondata = getResult();
    let keiro = jsondata["経路"];
    if (keiro.length > 0) {
        let last_ido = keiro[keiro.length - 1]["緯度"];
        let last_keido = keiro[keiro.length - 1]["経度"];
        console.log("緯度：" + ido + "|" + last_ido);
        console.log("経度：" + keido + "|" + last_keido);
        let d = distance(Number(ido), Number(keido), Number(last_ido), Number(last_keido));
        console.log("距離" + d);
        if (d < 1.5) {
            // 1.5m以下は追加しない。
            return;
        }
        if (d > 100) {
            // 100m以上は追加しない。
            return;
        }
    }
    jsondata["経路"].push({"緯度": Number(ido), "経度":Number(keido)})
    setResult(jsondata);
}

function handlePositon(pos) {
    //var result = document.querySelector("#result");
    let ido = pos.coords.latitude;
    let keido = pos.coords.longitude;
    addIdoKeido(ido, keido);
    clickUpdate();
}

function msg(m) {
    let msg_bar = document.querySelector("#msg_bar");
    msg_bar.value = m;
    msg_bar.focus();
}
