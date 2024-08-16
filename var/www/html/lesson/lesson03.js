
let gmap;

async function initMap() {
    //@ts-ignore
    const { Map } = await google.maps.importLibrary("maps");

    var ido = -34.397;
    var keido = 150.644;
    if (sessionStorage) {
        var lat = sessionStorage.getItem("latitude");
        if (lat) ido = Number(lat);
        var lng = sessionStorage.getItem("longitude");
        if (lng) keido = Number(lng);
    }

    gmap = new Map(document.getElementById("result"), {
        //mapTypeId: google.maps.MapTypeId.SATELLITE, 
        center: { lat: ido, lng: keido},
        zoom: 15,
    });
}

initMap();


var h = function(str) {
    if (str) {
        str = str.replace(/&/g, "&amp;");
        str = str.replace(/</g, "&lt;");
        str = str.replace(/>/g, "&gt;");
        return str;
    } else {
        return "";
    }
}

var count = 0;
function handlePositon(pos) {
    var result = document.querySelector("#result");
    var ido = pos.coords.latitude;
    var keido = pos.coords.longitude;
    var heading = pos.coords.heading;
    count ++;
    msg = `${count}: 緯度（${ido}）経度（${keido}）方角（${heading}）`;
    gmap.setCenter( { lat: Number(ido), lng: Number(keido) });
    //result.innerHTML = msg;
    if (sessionStorage) {
        sessionStorage.setItem("latitude", ido);
        sessionStorage.setItem("longitude", keido);
    }
}



window.onload = function() {

    var w_id;

    var result = document.querySelector("#result");

    var stopwatch = document.querySelector("#stopwatch");
    stopwatch.onclick = function() {
        navigator.geolocation.clearWatch(w_id);
    }
    if (navigator.geolocation) {
        w_id = navigator.geolocation.watchPosition(
            (pos) => { handlePositon(pos);}
            , function(err) {
                result.innerHTML("エラー：", err.code);
            }
            ,{
                timeout:5000,
                maximumAge: 0,
                enableHighAccuracy: true
            }
        );
    }
}