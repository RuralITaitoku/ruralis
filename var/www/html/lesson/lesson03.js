
let gmap;
let m_list = [];
async function initMap() {
    const { Map } = await google.maps.importLibrary("maps");

    var ido = -34.397;
    var keido = 150.644;
    var lat, lng;
    if (sessionStorage) {
        lat = sessionStorage.getItem("latitude");
        if (lat) ido = Number(lat);
        lng = sessionStorage.getItem("longitude");
        if (lng) keido = Number(lng);
    }

    gmap = new Map(document.getElementById("result"), {
        center: { lat: ido, lng: keido},
        zoom: 15,
    });
    if (lat) {
        addMaker(ido, keido, "現在地", "");
    }
}

initMap();


function clearMaker() {
    for( var i = 0; i < m_list.length; i++) {
        m_list[i].setMap(null);
    }
    m_list = [];
}
function addMaker(ido, keido, marker_title, marker_memo) {
    let marker = new google.maps.Marker({
        position:  { lat: Number(ido), lng: Number(keido)},
        map:gmap
    });
    let info = new google.maps.InfoWindow({
        headerContent: marker_title,
        content: marker_memo
    });
    marker.addListener("click", () => {
        info.open({
            anchor: marker,
            map:gmap,
        });
    });
    m_list.push(marker);

}

var count = 0;
function handlePositon(pos) {
    //var result = document.querySelector("#result");
    var ido = pos.coords.latitude;
    var keido = pos.coords.longitude;
    //var heading = pos.coords.heading;
    count ++;
    //msg = `${count}: 緯度（${ido}）経度（${keido}）方角（${heading}）`;
    gmap.setCenter( { lat: Number(ido), lng: Number(keido) });
    if (sessionStorage) {
        sessionStorage.setItem("latitude", ido);
        sessionStorage.setItem("longitude", keido);
    }
}


function showMemo() {
    let memo_list = [];
    if (localStorage) {
        let memo_item = localStorage.getItem("位置情報メモ");
        if (memo_item) {
            memo_list = JSON.parse(memo_item);
        }
    }
    let memo_result = "";
    for (let i = 0; i < memo_list.length; i++) {
        let subject = h(memo_list[i]["件名"]);
        memo_result += `<li><a href="#" class="show" data-id="${i}">${subject}</a>`;
        memo_result += ` <a href="#" class="del" data-id="${i}">×</a></li>`;
    }
    var result = document.querySelector("#list");
    if (result) {
        result.innerHTML = memo_result;
    }
    var subjects = document.querySelectorAll("#list a.del");
    for (var i = 0; i < subjects.length; i++) {
        subjects[i].addEventListener("click", clickDel, false);
    }
    var subjects = document.querySelectorAll("#list a.show");
    for (var i = 0; i < subjects.length; i++) {
        subjects[i].addEventListener("click", clickShow, false);
    }
}
function clickShow(e) {
    let id = e.target.getAttribute("data-id");
    let memo_list = [];
    if (localStorage) {
        let memo_item = localStorage.getItem("位置情報メモ");
        if (memo_item) {
            memo_list = JSON.parse(memo_item);
        }
        clearMaker();
        var title = memo_list[id]["件名"];
        var memo = memo_list[id]["メモ"];
        var ido = memo_list[id]["緯度"];
        var keido = memo_list[id]["経度"];
        addMaker(ido, keido, title, hh(memo));
        gmap.setCenter({ lat: Number(ido), lng: Number(keido)});
    }
}
function clickDel(e) {
    let id = e.target.getAttribute("data-id");
    let memo_list = [];
    if (localStorage) {
        let memo_item = localStorage.getItem("位置情報メモ");
        if (memo_item) {
            memo_list = JSON.parse(memo_item);
        }
        memo_list.splice(id, 1);
        localStorage.setItem("位置情報メモ", JSON.stringify(memo_list));
    }
    showMemo();
}



window.onload = function() {

    var w_id;

    var result = document.querySelector("#result");

    var stopwatch = document.querySelector("#stopwatch");
    stopwatch.onclick = function() {
        navigator.geolocation.clearWatch(w_id);
    }
    if (navigator.geolocation) {
        console.log("onload watchPosition");
        w_id = navigator.geolocation.watchPosition(
            (pos) => { handlePositon(pos);}
            , function(err) {
                result.innerHTML = 
                    "エラー：" + err.code + ":" + err.message;
            }
            ,{
                timeout:5000,
                maximumAge: 0,
                enableHighAccuracy: true
            }
        );
    }
    showMemo();
}