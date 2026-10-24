# Ghost Desktop

Um aplicativo útil para coisas inúteis.

(Antes se chamava Wabbe Control Center, mas esse nome parecia painel de controle de usina nuclear, então virou Ghost Desktop. Porque ele faz as coisas sumirem. Genial, eu sei.)

Basicamente, isso aqui nasceu porque eu queria controlar umas paradas do Windows sem ter que ficar caçando configuração escondida igual item raro em RPG ruim.

O app serve pra automatizar umas funções simples, tipo esconder ícones, esconder cursor, ativar modos diferentes e deixar o desktop com menos bagunça visual. Não vai mudar sua vida, não vai pagar boleto, não vai fazer café, mas pelo menos deixa o PC com uma vibe mais organizada.

Ainda em desenvolvimento, tem muitos probleminhas chatos mas to com preguiça pra arrumar, deixei o codigo nos arquivos caso alguma alma santa queira consertar.

## O que ele faz?

* Esconde os ícones da área de trabalho automaticamente
* Esconde o cursor quando ele fica parado
* Tem modos diferentes pra usar dependendo da situação
* Fica na bandeja do Windows
* Salva as configurações pra não ter que arrumar tudo de novo (agora de verdade, juro)
* Tenta ser bonito sem pesar igual app feito por empresa que odeia RAM

## Novidades (ou: coisas que eu deveria ter feito desde o começo)

* **Versão nova em Python**: a pasta `Ghost Desktop (Python)` é a versão atual, com uma interface que não parece ter saído do Windows 98. A pasta `Wabbe Control Center` é a versão antiga em C++, que fica aí por motivos sentimentais.
* **Organizador automático**: joga cada arquivo dos Downloads na pasta certa, separa os atalhos da área de trabalho em `Jogos` e `Apps` e arruma os arquivos soltos de Documentos. Ele não apaga nada, só move, porque eu não confio nem em mim mesmo. Instalador velho vai pra `_Para apagar`, e quem apaga é você.
* **Os ícones não somem mais na sua cara**: com um app aberto em janela, eles ficam quietinhos. Só somem com tudo minimizado ou com o app em tela cheia, que é quando ninguém tá olhando mesmo.
* **Adeus slider**: o tempo agora você digita ou escolhe numa lista, porque arrastar bolinha pra acertar "7 segundos" é um sofrimento que ninguém merece.

## Como usar essa bagaça

1. Instala o Python pelo [python.org](https://www.python.org/downloads/) e marca "Add python.exe to PATH" (sério, marca, senão nada funciona e a culpa vai ser minha)
2. Entra na pasta `Ghost Desktop (Python)` e roda o `instalar.bat` (só na primeira vez)
3. Roda o `iniciar.bat`
4. Pronto. Era isso.

Quer um `.exe` pra se sentir profissional? Roda o `build_exe.bat` e ele aparece em `dist\Wabbe.exe`.

As configurações ficam em `%APPDATA%\WabbeControlCenter\config.json`, caso você goste de mexer onde não deve.

## Por que existe?

Porque às vezes a gente perde mais tempo arrumando o PC do que usando ele.

Então eu fiz esse app pra resolver probleminhas específicos que provavelmente só incomodavam eu mesmo, mas agora estão organizados em uma interface bonitinha.

## Resumo

É isso.

Um aplicativo útil para coisas inúteis, feito com carinho, raiva, café imaginário e vontade de deixar o desktop menos feio.
