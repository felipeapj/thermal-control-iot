const char index_html[] = 
R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>Controle de temperatura</title>
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no"
      charset="UTF-8"
    />
    <style>
      * {
        font-family: "Arial", sans-serif;
      }
      .wrapper {
        width: 101px;
        height: 101px;
        border: 11px solid white;
        background-color: white;
        border-radius: 50%;
        clip-path: inset(50% 0px 0px 0px);
        transform: rotate(180deg);
      }
      .circle {
        width: 100px;
        height: 100px;
        border: 10px solid #369;
        border-radius: 50%;
        clip-path: inset(0px 0px 51% 0px);
      }
      body {
        margin: 0;
        display: flex;
        align-items: center;
        justify-content: center;
        flex-direction: column;
        height: 100vh;
        width: 100%;
        position: fixed;
      }

      .value-container {
        display: grid;
        grid-template-columns: 50px 120px 50px;
        grid-template-rows: auto;
        grid-template-areas: "min spinner plus";
        justify-items: center;
        align-items: center;
        gap: 10px
      }

      .item-min {
        grid-area: min;
      }

      .item-spinner {
        grid-area: spinner;
      }

      .item-plus {
        grid-area: plus;
      }

      button {
        width: 50px;
        height: 50px;
        border-radius: 50%;
        background-color: #369;
        border: none;
        color: white;
        text-align: center;
        text-decoration: none;
        font-size: 20px;
      }

      #currentTemp {
        z-index: 100;
      }
    </style>
  </head>

  <body>
    <h2>CONTROLE DE TEMPERATURA</h2>
    <h4 id="currentTemperature"></h4>
    <div class="value-container">
      <button class="item-min" onclick="changeTemp('-')">-</button>
      <h3 id="currentTemp" class="item-spinner"></h3>
      <div class="circle item-spinner"></div>
      <div class="wrapper item-spinner"></div>
      <button class="item-plus" onclick="changeTemp('+')">+</button>
    </div>
  </body>
  <script>
    const MIN_TEMP = 15;
    const MAX_TEMP = 30;
    var temp = 25;
    setInterval(() => {
      fetch("/get-temp").then((resp) => {
        if (resp.ok) {
          resp.text().then((body) => {
            const data = body.split('|');
            document.getElementById("currentTemperature").innerHTML = data[0];
            temp = parseInt(data[1]);
          });
        } else {
          document.getElementById("currentTemperature").innerHTML =
            "#Erro ao obter a temperatura#";
          clearInterval();
        }
      });
    }, 1000);

    changeTemp = (cmd) => {
      if (cmd == "-") {
        if (temp > 15) {
          temp -= 1;
        }
      } else {
        if (temp < 30) {
          temp += 1;
        }
      }
      setTemp();
    };

    setTemp = () => {
      const rotation = 180 + (180 / (MAX_TEMP - MIN_TEMP)) * (temp - MIN_TEMP);
      document.getElementsByClassName(
        "wrapper"
      )[0].style.WebkitTransitionDuration = "1s";
      document.getElementsByClassName(
        "wrapper"
      )[0].style.webkitTransform = `rotate(${rotation}deg)`;
      var text = `${temp}?? C`;
      if (temp == MIN_TEMP) {
        text = "TURBO";
      } else if (temp == MAX_TEMP) {
        text = "OFF";
      }
      document.getElementById("currentTemp").innerHTML = text;
      sendCommand();
    };

    sendCommand = () => {
      fetch(`/set-temp?temp=${temp}`)
        .then((resp) => {
          if (!resp.ok) {
            window.alert("Erro ao enviar o comando!");
            console.log(resp);
          }
          return resp.text();
        })
        .then((body) => {
          console.log(body);
        });
    };

    setTemp();
  </script>
</html>
)=====";
