use std::io::{self, Read, Write};
use std::mem::size_of;
use std::net::{SocketAddr, TcpListener, TcpStream};
use std::os::raw::c_char;
use std::sync::{Arc, Mutex};
use std::thread;
#[derive(Debug, PartialEq)]

struct Player {
    id: u64,
    x: f32,
    y: f32,
    z: f32,
}

struct UpdateClient {
    total_players: i32,
    players: [Player],
}

/*

Index 0 - Type Enum:
0 = Introduce new client
    i32 ID

1 = Remove client
    i32 ID

2 = Client transformation
    i32 ID
    f32 x
    f32 y
    f32 z

*/

#[allow(unreachable_code)]
fn handle_client(mut stream: TcpStream, clients: Arc<Mutex<Vec<Player>>>) -> io::Result<()> {
    let peer_addr = stream.peer_addr()?;
    let client_player_id = hash_ip_address(&peer_addr);
    println!("New connection: {}", peer_addr);

    let client = Player {
        id: client_player_id,
        x: 0.0,
        y: 0.0,
        z: 0.0,
    };
    clients.lock().unwrap().push(client);
    println!("Client ID: {}", clients.lock().unwrap().len());
    let mut buffer = [0; 1024];
    let nbytes = stream.read(&mut buffer)?;
    if nbytes == 0 {
        return Ok(()); // connection was closed
    }

    let received_data = String::from_utf8_lossy(&buffer[..nbytes]);
    println!("Received data: {}", received_data);

    // Echo the data back to the client
    stream.write_all(&buffer[..nbytes])?;

    loop {
        {
            let nbytes = stream.read(&mut buffer)?;
            if nbytes == 0 {
                return Ok(()); // connection was closed
            }
            let pos_x: [u8; 4] = buffer[0..4].try_into().expect("slice with incorrect length");
            let pos_y: [u8; 4] = buffer[4..8].try_into().expect("slice with incorrect length");
            let pos_z: [u8; 4] = buffer[8..12].try_into().expect("slice with incorrect length");
            let value = f32::from_le_bytes(pos_x);
            let value2 = f32::from_le_bytes(pos_y);
            let value3 = f32::from_le_bytes(pos_z);
            let mut clients_lock = clients.lock().unwrap();
            let client = match clients_lock.iter_mut().find(|player| player.id == client_player_id) {
                Some(client) => client,
                None => {
                    println!("hasd");
                    return Ok(());
                }
            };

            client.x = value;
            client.y = value2;
            client.z = value3;
            for player in clients_lock.iter() {
                println!("PLAYER{:?}", player);
            }
        }
        let mut buffer2 = [0; 1024];
        let buffer_offset = construct_client_update(&mut buffer, &clients);
        println!("Sent update to client: {}", buffer_offset);
        stream.write_all(&buffer[..buffer_offset])?;
    }

    Ok(())
}

fn main() -> io::Result<()> {
    // let mut ip_buffer = String::new();
    // let mut port_buffer = String::new();

    // println!("Enter machine IP address:");
    // std::io::stdin().read_line(&mut ip_buffer).unwrap();
    // println!("Enter machine IP address port:");
    // std::io::stdin().read_line(&mut port_buffer).unwrap();
    // println!("Connecting to {} on port {}", ip_buffer, port_buffer);
    // return Ok(());

    let listener = TcpListener::bind("192.168.38.164:6969")?;
    println!("Server listening on port 6969");

    let clients: Arc<Mutex<Vec<Player>>> = Arc::new(Mutex::new(Vec::new()));

    for stream in listener.incoming() {
        let clients_clone = clients.clone();
        println!("Active connections: {}", clients_clone.lock().unwrap().len());
        match stream {
            Ok(stream) => {
                thread::spawn(move || {
                    handle_client(stream, clients_clone);
                });
            }
            Err(e) => eprintln!("Failed to accept connection: {}", e),
        }
    }

    Ok(())
}

fn construct_client_update(buffer: &mut [u8], clients: &Arc<Mutex<Vec<Player>>>) -> usize {
    let clients_lock = clients.lock().unwrap();

    let type_enum: u8 = 1; // Assuming you have a protocol for this
    buffer[0] = type_enum;

    let number_of_players: u8 = clients_lock.len() as u8;
    buffer[1] = number_of_players;

    let mut offset = 2;
    for player in clients_lock.iter() {
        buffer[offset..(offset + 8)].copy_from_slice(&player.id.to_le_bytes());
        offset += 8;

        buffer[offset..(offset + 4)].copy_from_slice(&player.x.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.y.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.z.to_le_bytes());
        offset += 4;
    }

    offset // Return the total size of the constructed message
}

fn hash_ip_address(ip_address: &SocketAddr) -> u64 {
    let mut hasher = std::collections::hash_map::DefaultHasher::new();
    std::hash::Hash::hash(&ip_address, &mut hasher);
    std::hash::Hasher::finish(&hasher)
}
