
function w3_open() {
    document.getElementById("mySidebar").style.display = "block";
}

function w3_close() {
    document.getElementById("mySidebar").style.display = "none";
}

function getDataPointsFromCSV(csv, j, statelimit, vallimit) {//méthode qui sépare notre fichier csv pour renvoyer un tableau avec x et y
    var dataPoints = csvLines = points = []; //creations des tableaus
    csvLines = csv.split(/[\r?\n|\r|\n]+/); //expression regulière pour séparer le fichier par les retours à la ligne
    for (var i = 0; i < csvLines.length; i++)
        if (csvLines[i].length > 0) {
            points = csvLines[i].split(";");//sépare chaque ligne par ;
            if (statelimit == false) {//false = on construit un tableau avec les données
                dataPoints.push({
                    x: points[0] * 1000,//date
                    y: parseFloat(points[j])//valeur de la donnée
                });
            } else {//true = On construit un tableau avec notre limite (tracé sur seuil)
                dataPoints.push({
                    x: points[0] * 1000,//date
                    y: parseFloat(vallimit)//valeur limite
                });
            }
        }
    return dataPoints;
}

/*
window.onload = function () {
    var csvString = "";
    var xhttp = new XMLHttpRequest();//création de la requete 
    xhttp.open("GET", "liredata", false);//Get = HTTP_GET, liredata = nom de la requete, false = synchrone
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) { //200 = statut OK
            csvString = this.responseText; //stock le réponse du serveur dans le string csvString
        }
    };
    xhttp.send(); //Envoi de la requpete
    console.log("contenu JS :") //DBEUG
    console.log(csvString); //DEBUG

    var chart = new CanvasJS.Chart("chartContainer", {
        animationEnabled: true,//animation de la page lors du chargement
        animationDuration: 5000,//temps de l'animation
        zoomEnabled: true,//zoom-n-pan possible sur les graphiques
        theme: "dark2",//thème du graphique 
        title: {//titre
            text: "Evolution temporelle des angles d'inclinaison",
            fontFamily: "Calibri,Optima,Arial,sans-serif",
            verticalAlign: "top",
            horizontalAlign: "center"
        },
        axisX: {//axe x 
            title: "Time",
            crosshair: {
                enabled: true
            }
        },
        axisY: {//axe Y
            title: "Inclinaison angulaire",
            suffix: "°",
            includeZero: true
        },
        data: [{
            type: "spline",//type de ligne
            name: "Angle X",//nom dans la légende
            connectNullData: true,
            xValueType: "dateTime",//type de donnée
            xValueFormatString: "DD MMM YYYY HH:MM:ss",//formattage de données x
            yValueFormatString: "#,###.##°",//formattage de données y
            showInLegend: "true",//afficher la légende
            dataPoints: getDataPointsFromCSV(csvString, 1, false,)//tracé du graphique
        }, {
            type: "spline",
            name: "Angle Y",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#,###.##°",
            showInLegend: "true",
            dataPoints: getDataPointsFromCSV(csvString, 2, false,)
        }, {
            type: "spline",
            name: "Limite superieur",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#,###.##°",
            showInLegend: "true",
            markerType: "none",
            dataPoints: getDataPointsFromCSV(csvString, 2, true, 45.0)
        }, {
            type: "spline",
            name: "Limite inférieur",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#,###.##°",
            showInLegend: "true",
            markerType: "none",
            dataPoints: getDataPointsFromCSV(csvString, 2, true, -45.0)
        }]
    });
    chart.render();

    var chart2 = new CanvasJS.Chart("chartContainer2", {
        animationEnabled: true,
        animationDuration: 5000,
        zoomEnabled: true,
        theme: "dark2",
        title: {
            text: "Evolution temporelle de la température",
            fontFamily: "Calibri,Optima,Arial,sans-serif",
            verticalAlign: "top",
            horizontalAlign: "center"
        },
        axisX: {
            title: "Time",
            crosshair: {
                enabled: true
            }
        },
        axisY: {
            title: "Température",
            suffix: "°C",
            includeZero: true
        },
        data: [{
            type: "spline",
            name: "Température",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#,###.##°C",
            showInLegend: "true",
            dataPoints: getDataPointsFromCSV(csvString, 3, false,)
        }, {
            type: "spline",
            name: "Limite supérieur",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#,###.##°C",
            showInLegend: "true",
            markerType: "none",
            dataPoints: getDataPointsFromCSV(csvString, 3, true, 25.0)
        }, {
            type: "spline",
            name: "Limite inférieur",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#,###.##°C",
            showInLegend: "true",
            markerType: "none",
            dataPoints: getDataPointsFromCSV(csvString, 3, true, 5.0)
        }]
    });
    chart2.render();

    var chart3 = new CanvasJS.Chart("chartContainer3", {
        animationEnabled: true,//animation when the page refresh
        animationDuration: 5000,
        zoomEnabled: true,
        theme: "dark2",//chart's theme
        title: {
            text: "Evolution temporelle de la luminosité",//title
            fontFamily: "Calibri,Optima,Arial,sans-serif",//title's style
            verticalAlign: "top",
            horizontalAlign: "center"
        },
        axisX: {
            title: "Time",
            crosshair: {
                enabled: true
            }
        },
        axisY: {
            title: "Luminosité",
            suffix: "Lux",
            includeZero: true//0 is allowed
        },
        data: [{
            type: "spline",
            name: "Luminosité", //name in the legend
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#### Lux",//y data format
            showInLegend: "true",//include legend
            dataPoints: getDataPointsFromCSV(csvString, 4, false,)//fill the chart with data
        }, {
            type: "spline",
            name: "Limite",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            ValueFormatString: "###Lux",//y data format
            showInLegend: "true",
            markerType: "none",
            dataPoints: getDataPointsFromCSV(csvString, 4, true, 200)//set limit data
        }]
    });
    chart3.render();

    var chart4 = new CanvasJS.Chart("chartContainer4", {
        animationEnabled: true,//animation when the page refresh
        animationDuration: 5000,
        zoomEnabled: true,
        theme: "dark2",//chart's theme
        title: {
            text: "Evolution temporelle de la vitesse en Z",//title
            fontFamily: "Calibri,Optima,Arial,sans-serif",//title's style
            verticalAlign: "top",
            horizontalAlign: "center"
        },
        axisX: {
            title: "Time",
            crosshair: {
                enabled: true
            }
        },
        axisY: {
            title: "Vitesse",
            suffix: "cm/s",
            includeZero: true//0 is allowed
        },
        data: [{
            type: "spline",
            name: "Vitesse en Z", //name in the legend
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            yValueFormatString: "#### cm/s",//y data format
            showInLegend: "true",//include legend
            dataPoints: getDataPointsFromCSV(csvString, 5, false,)//fill the chart with data
        }, {
            type: "spline",
            name: "Limite",
            connectNullData: true,
            xValueType: "dateTime",
            xValueFormatString: "DD MMM YYYY HH:MM:ss",
            ValueFormatString: "#### cm/s",//y data format
            showInLegend: "true",
            markerType: "none",
            dataPoints: getDataPointsFromCSV(csvString, 5, true, 242)//set limit data
        }]
    });
    chart4.render();
}*/