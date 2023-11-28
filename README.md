# Regra de Simpson

Este projeto é uma implementação utilizando programação concorrente para a regra de Simpson, método utilizado para aproximar integrais definidas numericamente.

## Sobre este projeto

Utilizando C, construímos duas versões da regra de Simpson: Uma sequencial e outra concorrente (utilizando a biblioteca pthreads), visando testar a diferença de tempo de execução das duas versões. A versão da regra utilizada neste projeto foi a mais simples, sem os mecanismos para correção de erros que versões mais potentes do método possuem, já que o foco do projeto é a concorrência. O algoritmo recebe uma função (que terá a integral aproximada), um limite inferior de integração, um limite superior de integração e a quantidade de iterações que deve realizar. Quanto maior a quantidade de iterações, menor se torna o erro, já que a aproximação (realizada através de parábolas) se torna cada vez mais específica, assim como na regra dos trapézios. Para funções polinomiais com grau maior do que 2, o algoritmo já não funciona tão corretamente, devido à dificuldade de aproximar funções mais complexas através de parábolas.

## Sobre a implementação concorrente

Para a implementação concorrente, optamos por utilizar a mesma função sequencial, apenas com as mudanças para acrescentar o resultado local em uma variável resultado global, dividindo os intervalos de execução entre as threads. Assim, por exemplo, se integrarmos x com a = 0 e b = 20 e 4 threads, a primeira thread irá integrar utilizando a regra de Simpson normalmente entre 0 e 5, a segunda entre 5 e 10, a terceira entre 10 e 15 e a quarta entre 15 e 20. Ao final, somando tudo, teremos o resultado esperado. Isso pode ser feito porque a integração é entendida como a “área embaixo da curva”, e a área de uma região grande é a soma da área de suas regiões menores.

Além disso, tivemos grandes problemas com os pontos flutuantes. Ao multiplicar muitas vezes por números com pontos flutuantes, os erros se acumulavam, gerando resultados muito incorretos. Para mitigar isso, optamos por limitar os valores possíveis de n para a implementação concorrente através de um fator multiplicador par, para garantir que não teríamos dízimas ou valores muito estranhos ao calcular o tamanho do intervalo para as threads. Para descobrir a quantidade de iterações totais da versão concorrente, utlize a conta: fator multiplicador * (b - a)


## Como utilizar
### Sequencial
Clone o repositório ou faça download do arquivo separadamente.

Compile o arquivo utilizando o comando gcc SimpsonSequencial.c -o "nome que você deseja"

Execute o arquivo utilizando .\"nome do arquivo" "limite inferior" "limite superior" "quantidade de iterações"

### Concorrente
Clone o repositório ou faça download do arquivo separadamente.

Compile o arquivo utilizando o comando gcc SimpsonConcorrente.c -o "nome que você deseja"

Execute o arquivo utilizando .\"nome do arquivo" "limite inferior" "limite superior" "número de threads" "fator multiplicador"

### Importante
A função precisa ser alterada manualmente no código em ambos os casos, basta alterar o return da função funcao.

O fator multiplicador deve ser par, não nulo e positivo para que o funcionamento seja adequado

O código está inicialmente integrando a função f(x) = x

Para as versões que testam tempo, o processo é o mesmo, apenas alterando o nome do arquivo a ser compilado


## Integrantes
Pedro Mion e Daniel Machado

## Tecnologias
- C
- Biblioteca pthreads