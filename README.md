# Wabbe Control Center

Um aplicativo útil para coisas inúteis.

Basicamente, isso aqui nasceu porque eu queria controlar umas paradas do Windows sem ter que ficar caçando configuração escondida igual item raro em RPG ruim.

O app serve pra automatizar umas funções simples, tipo esconder ícones, esconder cursor, ativar modos diferentes e deixar o desktop com menos bagunça visual. Não vai mudar sua vida, não vai pagar boleto, não vai fazer café, mas pelo menos deixa o PC com uma vibe mais organizada.

Ainda em desenvolvimento, tem muitos probleminhas chatos mas to com preguiça pra arrumar, deixei o codigo nos arquivos caso alguma alma santa queira consertar.

## O que ele faz?

* Esconde os ícones da área de trabalho automaticamente
* Esconde o cursor quando ele fica parado
* Tem modos diferentes pra usar dependendo da situação
* Fica na bandeja do Windows
* Salva as configurações pra não ter que arrumar tudo de novo
* Tenta ser bonito sem pesar igual app feito por empresa que odeia RAM

## Versão nova: Ghost Desktop (Python)

A pasta **`Ghost Desktop (Python)`** é a versão nova, com visual bem melhor (CustomTkinter) e que finalmente salva as configurações direito. A pasta `Wabbe Control Center` é a versão antiga em C++.

Novidades:

* **Organizador automático**: arruma Downloads (por tipo de arquivo), Área de Trabalho (atalhos de jogos em `Jogos`, programas em `Apps`) e arquivos soltos de Documentos. Nunca apaga nada, só move. Instaladores velhos vão para `_Para apagar` depois de X dias.
* **Ícones não somem com app aberto em janela**: só somem com tudo minimizado ou com o app maximizado/em tela cheia.
* **Tempo em caixinha**: dá para digitar os segundos ou escolher numa lista, em vez de ficar arrastando.

### Como rodar

1. Instale o Python pelo [python.org](https://www.python.org/downloads/) (marque "Add python.exe to PATH").
2. Dentro de `Ghost Desktop (Python)`, rode `instalar.bat` (só na primeira vez).
3. Rode `iniciar.bat`.

Quer um `.exe`? Rode `build_exe.bat` e ele aparece em `dist\Wabbe.exe`.

Configurações ficam em `%APPDATA%\WabbeControlCenter\config.json`.

## Por que existe?

Porque às vezes a gente perde mais tempo arrumando o PC do que usando ele.

Então eu fiz esse app pra resolver probleminhas específicos que provavelmente só incomodavam eu mesmo, mas agora estão organizados em uma interface bonitinha.

## Resumo

É isso.

Um aplicativo útil para coisas inúteis, feito com carinho, raiva, café imaginário e vontade de deixar o desktop menos feio.
