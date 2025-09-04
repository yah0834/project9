프로젝트9 (야구게임) 구현과정 정리:
접속 관리 -> 채팅 입력 -> 서버 검증/판정 -> 결과 브로드캐스트 -> 승리/무승부 처리 -> 게임 리셋

플레이어 접속 처리: OnPostLogin -> PlayerState 이름 부여, 접속 알림 멀티캐스트
UI/채팅 위젯: ChatInput 위젯 생성, Enter 입력 시 서버 RPC 호출
서버 RPC: 메시지 전달 -> 숫자 유효성 검사(길이3, 1~9, 중복X)
암호 숫자 생성: 1~9 배열 랜덤 셔플, 중복 없는 3자리 뽑기
판정 로직: Strike / Ball / OUT 계산, 결과 문자열 생성
시도 관리: PlayerState에 현재/최대 시도 횟수 저장 및 증가
승리 조건: 3 Strike ->승자 알림, 게임 리셋
무승부 조건: 전원 시도 소진 시 -> 무승부 알림, 게임 리셋
네트워크 동기화: PlayerState 변수(이름, 시도 횟수) 및 NotificationText Replicate
클라이언트 표시: Client RPC로 채팅/판정 결과 전파, UI에 출력
