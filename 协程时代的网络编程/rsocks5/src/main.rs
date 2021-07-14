use byteorder::{BigEndian, ByteOrder};

use std::io::Result;
use std::net::SocketAddr;
use std::net::{IpAddr, Ipv4Addr};

use tokio::io::{AsyncRead, AsyncReadExt, AsyncWrite, AsyncWriteExt};
use tokio::net::{TcpListener, TcpStream};

async fn copy<T: AsyncRead + Unpin, U: AsyncWrite + Unpin>(mut sock1: T, mut sock2: U) -> Result<()> {
    let mut buff = [0; 1024];
    loop {
        let n = sock1.read(&mut buff[..]).await?;
        if n == 0 {
            break;
        }

        sock2.write_all(&buff[..n]).await?;
    }

    return Ok(());
}

async fn socks5(mut local_socket: TcpStream) -> Result<()> {
    let mut buff = [0; 1024];

    // 接收05 01 00
    let n = local_socket.read(&mut buff).await?;

    if n < 3 || 2 + (buff[1] as usize) != n || buff[0] != b'\x05' {
        println!("invalid 05 01 00 length");
        return Ok(());
    }

    // 回复05 00
    local_socket.write_all(b"\x05\x00").await?;

    // 接收05 01 00 01/03 addr port
    let n = local_socket.read(&mut buff).await?;
    if n <= 4 {
        println!("invalid domain request: {:?}", &buff[..n]);
        return Ok(());
    }

    if buff[1] != 1 { // 只实现了cmd=1，即connect
        println!("cmd={} is not supported", buff[1]);
        buff[1] = b'\x07'; // command not supported
        local_socket.write_all(&mut buff[..n]).await?;
        return Ok(());
    }

    let addr; // 解析出来的地址保存在这里
    match buff[3] {
        1 => { // ipv4格式的地址: 05 01 00 01 ip[4] port[2]
            if n != 10 {
                println!("invalid address for ipv4: {:?}", &buff[..n]);
                return Ok(());
            }

            let dest_addr = IpAddr::V4(Ipv4Addr::new(buff[4], buff[5], buff[6], buff[7]));
            let dest_port =BigEndian::read_u16(&buff[8..]);
            addr = SocketAddr::new(dest_addr, dest_port).to_string();
        }
        3 => { // 域名: 05 01 00 03 host_len host[host_len] port[2]
            let offset = 4 + 1 + (buff[4] as usize);
            if offset + 2 != n {
                println!("invalid len for address type 3: {:?}", &buff[..n]);
                return Ok(());
            }
            let dest_port = BigEndian::read_u16(&buff[offset..]);
            let mut dest_addr = std::str::from_utf8(&buff[5..offset]).unwrap().to_string();
            dest_addr.push_str(":");
            dest_addr.push_str(&dest_port.to_string());
            println!("dest addr: {:?}", &dest_addr);
            addr = dest_addr;
        }
        _ => { // 其它的type暂时不支持，有兴趣自己加
            println!("address type={} not supported", buff[3]);
            let bytes = [b'\x05', b'\x08', b'\x00', b'\x01', b'\x00', b'\x00', b'\x00', b'\x00', b'\x00', b'\x00'];
            local_socket.write_all(&bytes[..]).await?;
            return Ok(());
        }
    }

    let remote_socket = TcpStream::connect(addr).await?;

    let bytes = [b'\x05', b'\x00', b'\x00', b'\x01', b'\x00', b'\x00', b'\x00', b'\x00', b'\x00', b'\x00'];
    local_socket.write_all(&bytes[..]).await?;

    let (ri, wi) = local_socket.into_split();
    let (ro, wo) = remote_socket.into_split();

    tokio::spawn(async move {
        copy(ro, wi).await.unwrap();
    });

    tokio::spawn(async move {
        copy(ri, wo).await.unwrap();
    });

    return Ok(());
}

async fn listener() -> Result<()> {
    let addr = "127.0.0.1:10001".to_string().parse::<SocketAddr>().unwrap();
    let tl: TcpListener = TcpListener::bind(&addr).await?;

    loop {
        let (socket, _) = tl.accept().await?;
        tokio::spawn(async move {
            socks5(socket).await.unwrap();
        });
    }
}

#[tokio::main]
async fn main() {
    //tokio::spawn(async {listener().await.unwrap();});
    listener().await.unwrap();
}
