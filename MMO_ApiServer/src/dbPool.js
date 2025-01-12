// 필요한 라이브러리 가져오기
const sql = require('mssql');

// 전역 커넥션 풀링
const poolDBConn = [];

// 데이터베이스 연결 설정
const config = {
  user: 'qornwh', // 데이터베이스 사용자 이름
  password: '123456', // 데이터베이스 비밀번호
  server: 'localhost', // 데이터베이스 서버 주소
  database: 'BSGameServerDB', // 데이터베이스 이름
  options: {
    encrypt: true, // 사용 중인 SQL Server 버전에 따라 변경 필요
    trustServerCertificate: true, // 개발 환경에서만 true 설정
  },
};

async function popConn() {
  if (poolDBConn.length == 0) {
    try {
      return await sql.connect(config);
    } catch (error) {
      console.error('', err);
      throw err;
    }
  }
  return poolDBConn.pop();
}

function pushConn(dbConn) {
  poolDBConn.push(dbConn);
}

module.exports = {
  popConn: popConn,
  pushConn: pushConn,
};
