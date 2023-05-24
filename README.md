# Projeto 1 de SO

- Desenvolver aplicações concorrentes que acessam recursos compartilhados.

```
O Professor Campiolo reservou uma sala de estudos em grupos para os alunos da disciplina de Sistemas
Operacionais. Os alunos de SO podem usar essa sala acompanhados de um estudante-monitor.

No entanto, há três restrições para uso da sala:
  1. deve-se ter um estudante-monitor para cada X alunos de SO (X é o número de estudantes por grupo).
  2. estudantes-monitores podem deixar a sala se a restrição (1) for mantida.
  3. Alunos de SO podem entrar na sala se a restrição (1) for mantida.
  
  Os estudantes-monitores podem tentar deixar a sala a qualquer momento, mas não conseguirão se a restrição (1)
não for mantida. Os alunos que chegarem para usar a sala, devem entrar imediatamente se a restrição (1) se
mantém.
  O professor Campiolo abrirá a sala para o início da entrada e avisará quando não poderá mais entrar alunos e
estudantes-monitores. Quando a sala estiver vazia, o professor Campiolo fechará a sala.
Considerações:
  • tipos de threads: Professor, Alunos de SO, estudantes-monitores.
  • Professor executa as ações: abrirSala, avisarAlunos, avisarEstudantesMonitores, fecharSala.
  • Alunos de SO executam as ações: entrarSala, sairSala, estudar.
  • Estudantes-monitores executam as ações: entrarSala, sairSala,supervisionarAlunos.
  • A turma de Sistemas Operacionais tem N Alunos. Cada grupo tem X alunos. Há K estudantesmonitores. Use constantes/variáveis no código para testes com diferentes valores.
  • exiba mensagens para mostrar as ações, por exemplo, alunoSO_1 entra na sala, estudanteMonitor_1 saiu da sala, professor abre a sala, estudanteMonitor_2 esperando para sair, alunoSO  esperando para entrar, e assim por diante.
```
