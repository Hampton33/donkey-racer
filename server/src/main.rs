use std::io::{self, Read, Write};
use std::net::{SocketAddr, TcpListener, TcpStream};
use std::sync::{Arc, Mutex};
use std::thread;
#[derive(Debug, PartialEq)]

struct Player {
    id: u64,
    x: f32,
    y: f32,
    z: f32,
    rotation_x: f32,
    rotation_y: f32,
    rotation_z: f32,
}

const IP_ADDRESS: &str = "0.0:8080"; // ip:port

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
        rotation_x: 0.0,
        rotation_y: 0.0,
        rotation_z: 0.0,
    };
    {
        clients.lock().unwrap().push(client);
        println!("Client ID: {}", clients.lock().unwrap().len());
    }

    let mut buffer = [0u8; 1024];
    let offset = 0;
    buffer[offset..(offset + 8)].copy_from_slice(&client_player_id.to_le_bytes());
    stream.write_all(&buffer[..8])?;

    loop {
        let _nbytes = match stream.read(&mut buffer) {
            Ok(n) => n,
            Err(_) => {
                println!("Connection closed XD");
                break;
            }
        };
        let recv_type: [u8; 4] = buffer[0..4]
            .try_into()
            .expect("slice with incorrect length");

        let player_id: [u8; 8] = buffer[4..12]
            .try_into()
            .expect("slice with incorrect length");
        let id = u64::from_le_bytes(player_id);

        let pos_x: [u8; 4] = buffer[12..16]
            .try_into()
            .expect("slice with incorrect length");
        let pos_y: [u8; 4] = buffer[16..20]
            .try_into()
            .expect("slice with incorrect length");
        let pos_z: [u8; 4] = buffer[20..24]
            .try_into()
            .expect("slice with incorrect length");

        let x = f32::from_le_bytes(pos_x);
        let y = f32::from_le_bytes(pos_y);
        let z = f32::from_le_bytes(pos_z);

        let rot_x: [u8; 4] = buffer[24..28]
            .try_into()
            .expect("slice with incorrect length");
        let rot_y: [u8; 4] = buffer[28..32]
            .try_into()
            .expect("slice with incorrect length");
        let rot_z: [u8; 4] = buffer[32..36]
            .try_into()
            .expect("slice with incorrect length");

        let rotation_x = f32::from_le_bytes(rot_x);
        let rotation_y = f32::from_le_bytes(rot_y);
        let rotation_z = f32::from_le_bytes(rot_z);
        println!(
            "Received: RECEVE TYPE: {:?}\n PLAYER ID;{:?} POSITION: {:?} {:?} {:?}\n ROTATION: {:?} {:?} {:?}\n",
            recv_type, id, x, y, z, rotation_x, rotation_y, rotation_z
        );
        {
            let mut clients_lock = clients.lock().unwrap();
            println!("clients size: {}", clients_lock.len());
            let client = match clients_lock
                .iter_mut()
                .find(|player| player.id == client_player_id)
            {
                Some(client) => client,
                None => {
                    break;
                }
            };

            client.x = x;
            client.y = y;
            client.z = z;

            client.rotation_x = rotation_x;
            client.rotation_y = rotation_y;
            client.rotation_z = rotation_z;
        }

        let buffer_offset = construct_client_update(&mut buffer, clients.clone());
        stream.write_all(&buffer[..buffer_offset])?;
    }
    let mut clients_lock = clients.lock().unwrap();
    clients_lock.retain(|player| player.id != client_player_id);
    for player in clients_lock.iter() {
        println!("PLAYER{:?}", player);
    }
    println!("Exiting client thread for client: {}", client_player_id);
    Ok(())
}

fn main() -> io::Result<()> {
    let listener = TcpListener::bind(IP_ADDRESS)?;
    println!("Listener on ip: {}", IP_ADDRESS);

    let clients: Arc<Mutex<Vec<Player>>> = Arc::new(Mutex::new(Vec::new()));

    for stream in listener.incoming() {
        let clients_clone = clients.clone();
        println!(
            "Active connections: {}",
            clients_clone.lock().unwrap().len()
        );
        match stream {
            Ok(stream) => {
                thread::spawn(move || {
                    let _ = handle_client(stream, clients_clone);
                });
            }
            Err(e) => eprintln!("Failed to accept connection: {}", e),
        }
    }

    Ok(())
}

fn construct_client_update(buffer: &mut [u8], clients: Arc<Mutex<Vec<Player>>>) -> usize {
    let clients_lock = clients.lock().unwrap();

    let mut offset = 0;

    let type_enum: i32 = 1;
    buffer[offset..(offset + 4)].copy_from_slice(&type_enum.to_le_bytes());
    offset += 4;

    let number_of_players: i32 = clients_lock.len() as i32;
    buffer[offset..(offset + 4)].copy_from_slice(&number_of_players.to_le_bytes());
    offset += 4;

    for player in clients_lock.iter() {
        println!("Sending update for player {:?}", player);
        buffer[offset..(offset + 8)].copy_from_slice(&player.id.to_le_bytes());
        offset += 8;

        buffer[offset..(offset + 4)].copy_from_slice(&player.x.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.y.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.z.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.rotation_x.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.rotation_y.to_le_bytes());
        offset += 4;

        buffer[offset..(offset + 4)].copy_from_slice(&player.rotation_z.to_le_bytes());
        offset += 4;
    }

    offset
}

fn hash_ip_address(ip_address: &SocketAddr) -> u64 {
    let mut hasher = std::collections::hash_map::DefaultHasher::new();
    std::hash::Hash::hash(&ip_address, &mut hasher);
    std::hash::Hasher::finish(&hasher)
}
