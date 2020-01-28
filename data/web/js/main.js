import {SmogolClient} from "./smogolClient.js";

// let a = new SmogolClient("http://10.1.1.10/api");
let a = new SmogolClient("/api");
a.onOnline(function(){console.log("online!");});
a.onNewData(data => 
{
    let elPm1 = document.getElementById("pm1");
    elPm1.innerText = data.pm1;
    let elPm25 = document.getElementById("pm25");
    elPm25.innerText = data.pm25;
    let elPm10 = document.getElementById("pm10");
    elPm10.innerText = data.pm10;

    let pm25Quality = 0;
    let pm10Quality = 0;
    elPm25.classList.remove("pm-normal", "pm-warning", "pm-bad");
    if (data.pm25 >= 25)
    {
        elPm25.classList.add("pm-bad");
        pm25Quality = 2;
    }
    else if (data.pm25 >= 10)
    {
        elPm25.classList.add("pm-warning");
        pm25Quality = 1;
    }
    else
    {
        elPm25.classList.add("pm-normal");
    }

    elPm10.classList.remove("pm-normal", "pm-warning", "pm-bad");
    if (data.pm10 >= 50)
    {
        elPm10.classList.add("pm-bad");
        pm10Quality = 2;
    }
    else if (data.pm10 >= 20)
    {
        elPm10.classList.add("pm-warning");
        pm10Quality = 1;
    }
    else
    {
        elPm10.classList.add("pm-normal");
    }

    let elQuality = document.getElementById("quality");
    elQuality.classList.remove("pm-normal", "pm-warning", "pm-bad");
    if (pm10Quality >= 2 || pm25Quality >= 2)
    {
        elQuality.classList.add("pm-bad");
        elQuality.innerText = "BAD";
    }
    else if (pm10Quality >= 1 || pm25Quality >= 1)
    {
        elQuality.classList.add("pm-warning");
        elQuality.innerText = "MEDIOCRE";
    }
    else
    {
        elQuality.classList.add("pm-normal");
        elQuality.innerText = "GOOD";
    }
});
a.onOnline(() =>
{
    let elOnline = document.getElementById("online");
    elOnline.innerText = "ONLINE";
    elOnline.classList.remove("online", "offline");
    elOnline.classList.add("online");
})
a.onOffline(() =>
{
    let elOnline = document.getElementById("online");
    elOnline.innerText = "OFFLINE";
    elOnline.classList.remove("online", "offline");
    elOnline.classList.add("offline");
})
a.start();
