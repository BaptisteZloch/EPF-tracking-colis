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