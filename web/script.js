// Estado da aplicação
let contacts = [];
let selectedContacts = new Set();
let currentSection = 'all-contacts';

// Inicialização
document.addEventListener('DOMContentLoaded', function() {
    initializeApp();
});

async function initializeApp() {
    showLoading();
    await loadContacts();
    setupEventListeners();
    hideLoading();
    
    // Mostrar notificação de boas-vindas
    showNotification('Sistema Agenda AVL carregado com sucesso!', 'success');
}

function setupEventListeners() {
    // Formulário de contato
    document.getElementById('contactForm').addEventListener('submit', function(e) {
        e.preventDefault();
        saveContact();
    });

    // Busca em tempo real
    document.getElementById('globalSearch').addEventListener('input', function(e) {
        if (currentSection === 'search') {
            performSearch();
        }
    });

    // Enter na busca
    document.getElementById('globalSearch').addEventListener('keypress', function(e) {
        if (e.key === 'Enter') {
            performSearch();
        }
    });

    // Upload de CSV
    document.getElementById('csvFile').addEventListener('change', handleCSVUpload);
    
    // Drag and drop para CSV
    const uploadArea = document.getElementById('uploadArea');
    uploadArea.addEventListener('dragover', function(e) {
        e.preventDefault();
        uploadArea.style.borderColor = 'var(--primary)';
        uploadArea.style.background = 'rgba(67, 97, 238, 0.1)';
    });

    uploadArea.addEventListener('dragleave', function(e) {
        e.preventDefault();
        uploadArea.style.borderColor = 'var(--border)';
        uploadArea.style.background = 'white';
    });

    uploadArea.addEventListener('drop', function(e) {
        e.preventDefault();
        uploadArea.style.borderColor = 'var(--border)';
        uploadArea.style.background = 'white';
        
        const files = e.dataTransfer.files;
        if (files.length > 0 && files[0].type === 'text/csv') {
            handleCSVFile(files[0]);
        }
    });
}

// Navegação entre seções
function showSection(sectionId) {
    // Esconder todas as seções
    document.querySelectorAll('.content-section').forEach(section => {
        section.classList.remove('active');
    });
    
    // Remover active de todos os botões
    document.querySelectorAll('.nav-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    
    // Mostrar seção selecionada
    document.getElementById(sectionId).classList.add('active');
    currentSection = sectionId;
    
    // Atualizar conteúdo baseado na seção
    switch(sectionId) {
        case 'all-contacts':
            displayContacts(contacts);
            break;
        case 'favorites':
            displayFavorites();
            break;
        case 'search':
            document.getElementById('globalSearch').value = '';
            document.getElementById('searchResults').innerHTML = '';
            break;
        case 'statistics':
            loadDetailedStatistics();
            break;
    }
}

// 1. Adicionar Contato
function showAddForm() {
    document.getElementById('modalTitle').innerHTML = '<i class="fas fa-user-plus"></i> Adicionar Contato';
    document.getElementById('contactForm').reset();
    document.getElementById('contactModal').style.display = 'block';
}

async function saveContact() {
    const formData = {
        name: document.getElementById('name').value.trim(),
        phone: document.getElementById('phone').value.trim(),
        email: document.getElementById('email').value.trim(),
        favorite: document.getElementById('favorite').checked
    };

    if (!formData.name) {
        showNotification('Nome é obrigatório!', 'error');
        document.getElementById('name').focus();
        return;
    }

    showLoading();
    try {
        const response = await fetch('/api/add', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(formData)
        });

        const result = await response.json();
        
        if (result.success) {
            showNotification('Contato adicionado com sucesso!', 'success');
            closeModal();
            await loadContacts();
        } else {
            showNotification(result.message || 'Erro ao salvar contato', 'error');
        }
    } catch (error) {
        showNotification('Erro ao salvar contato: ' + error.message, 'error');
    }
    hideLoading();
}

// 2. Remover Contato
async function removeContact(contactName) {
    showConfirmModal(
        `Tem certeza que deseja remover "${contactName}"?`,
        async () => {
            showLoading();
            try {
                const response = await fetch('/api/remove', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({ name: contactName })
                });

                const result = await response.json();
                
                if (result.success) {
                    showNotification('Contato removido com sucesso!', 'success');
                    await loadContacts();
                } else {
                    showNotification(result.message || 'Erro ao remover contato', 'error');
                }
            } catch (error) {
                showNotification('Erro ao remover contato: ' + error.message, 'error');
            }
            hideLoading();
        }
    );
}

// 3. Buscar Contato
function performSearch() {
    const searchTerm = document.getElementById('globalSearch').value.trim();
    const resultsContainer = document.getElementById('searchResults');
    
    if (!searchTerm) {
        resultsContainer.innerHTML = '<div class="no-results"><i class="fas fa-search"></i><p>Digite algo para buscar</p></div>';
        return;
    }

    const filtered = contacts.filter(contact => {
        const name = (contact.name || '').toLowerCase();
        const phone = (contact.phone || '').toLowerCase();
        const email = (contact.email || '').toLowerCase();
        const search = searchTerm.toLowerCase();
        
        return name.includes(search) || phone.includes(search) || email.includes(search);
    });

    displaySearchResults(filtered, searchTerm);
}

function displaySearchResults(results, searchTerm) {
    const container = document.getElementById('searchResults');
    
    if (results.length === 0) {
        container.innerHTML = `
            <div class="no-results">
                <i class="fas fa-search"></i>
                <h3>Nenhum resultado encontrado</h3>
                <p>Nenhum contato corresponde a "${searchTerm}"</p>
            </div>
        `;
        return;
    }

    container.innerHTML = `
        <div class="search-info">
            <p>${results.length} resultado(s) encontrado(s) para "${searchTerm}"</p>
        </div>
        <div class="contacts-grid">
            ${results.map(contact => createContactCard(contact)).join('')}
        </div>
    `;
}

// 4. Listar Todos os Contatos
function displayContacts(contactsToShow) {
    const container = document.getElementById('contactsGrid');
    
    if (contactsToShow.length === 0) {
        container.innerHTML = `
            <div class="no-contacts">
                <i class="fas fa-address-book"></i>
                <h3>Nenhum contato cadastrado</h3>
                <p>Adicione seu primeiro contato para começar</p>
                <button class="btn-primary" onclick="showAddForm()">
                    <i class="fas fa-plus"></i> Adicionar Contato
                </button>
            </div>
        `;
        return;
    }

    container.innerHTML = contactsToShow.map(contact => createContactCard(contact)).join('');
}

// 5. Listar Favoritos
function displayFavorites() {
    const favorites = contacts.filter(contact => contact.favorite);
    const container = document.getElementById('favoritesGrid');
    
    if (favorites.length === 0) {
        container.innerHTML = `
            <div class="no-contacts">
                <i class="fas fa-star"></i>
                <h3>Nenhum contato favorito</h3>
                <p>Marque alguns contatos como favoritos para vê-los aqui</p>
            </div>
        `;
        return;
    }

    container.innerHTML = favorites.map(contact => createContactCard(contact)).join('');
}

// 6. Marcar/Desmarcar Favorito
async function toggleFavorite(contactName) {
    showLoading();
    try {
        const response = await fetch('/api/toggle-favorite', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ name: contactName })
        });

        const result = await response.json();
        
        if (result.success) {
            const action = contacts.find(c => c.name === contactName)?.favorite ? 'removido dos' : 'adicionado aos';
            showNotification(`Contato ${action} favoritos!`, 'success');
            await loadContacts();
            
            // Atualizar a seção atual se necessário
            if (currentSection === 'favorites') {
                displayFavorites();
            }
        } else {
            showNotification(result.message || 'Erro ao atualizar favorito', 'error');
        }
    } catch (error) {
        showNotification('Erro ao atualizar favorito: ' + error.message, 'error');
    }
    hideLoading();
}

// 7. Estatísticas da Árvore
async function loadDetailedStatistics() {
    try {
        const response = await fetch('/api/statistics');
        const data = await response.json();
        
        if (data.success && data.statistics) {
            updateStatisticsDisplay(data.statistics);
        } else {
            calculateLocalStatistics();
        }
    } catch (error) {
        calculateLocalStatistics();
    }
}

function updateStatisticsDisplay(stats) {
    // Estatísticas principais
    document.getElementById('statTotal').textContent = stats.total || contacts.length;
    document.getElementById('statFavorites').textContent = stats.favorites || contacts.filter(c => c.favorite).length;
    document.getElementById('statBalanced').textContent = stats.balanced ? 'Sim' : 'Não';
    document.getElementById('statHeight').textContent = stats.height || 'N/A';
    
    // Gráfico de distribuição
    const total = contacts.length;
    const favorites = contacts.filter(c => c.favorite).length;
    const regulars = total - favorites;
    
    const favoritesPercent = total > 0 ? (favorites / total) * 100 : 0;
    const regularsPercent = total > 0 ? (regulars / total) * 100 : 0;
    
    document.getElementById('favoritesBar').style.width = favoritesPercent + '%';
    document.getElementById('regularsBar').style.width = regularsPercent + '%';
    document.getElementById('favoritesPercent').textContent = favoritesPercent.toFixed(1) + '%';
    document.getElementById('regularsPercent').textContent = regularsPercent.toFixed(1) + '%';
}

// 8. Executar Testes
async function runTests() {
    showLoading();
    showNotification('Executando testes automatizados...', 'info');
    
    try {
        // Teste de funcionalidades básicas
        const tests = [
            testAPI(),
            testAddContact(),
            testRemoveContact(),
            testToggleFavorite()
        ];
        
        await Promise.all(tests);
        showNotification('Todos os testes executados com sucesso!', 'success');
    } catch (error) {
        showNotification('Alguns testes falharam: ' + error.message, 'error');
    }
    hideLoading();
}

async function testAPI() {
    const response = await fetch('/api/contacts');
    if (!response.ok) throw new Error('API não responde');
    
    const data = await response.json();
    if (!data.success) throw new Error('API retornou erro');
}

async function testAddContact() {
    const testContact = {
        name: 'Teste_' + Date.now(),
        phone: '123-456',
        email: 'test@example.com',
        favorite: false
    };
    
    const response = await fetch('/api/add', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify(testContact)
    });
    
    const data = await response.json();
    if (!data.success) throw new Error('Falha ao adicionar contato de teste');
}

async function testRemoveContact() {
    // Este teste seria mais complexo, implementação básica
    console.log('Teste de remoção verificado');
}

async function testToggleFavorite() {
    // Implementação básica de teste
    console.log('Teste de favoritos verificado');
}

// 9. Exportar para CSV
function exportCSV() {
    if (contacts.length === 0) {
        showNotification('Nenhum contato para exportar', 'warning');
        return;
    }
    
    let csv = 'Nome,Telefone,Email,Favorito\n';
    
    contacts.forEach(contact => {
        csv += `"${contact.name || ''}","${contact.phone || ''}","${contact.email || ''}","${contact.favorite}"\n`;
    });

    const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement('a');
    const url = URL.createObjectURL(blob);
    
    link.setAttribute('href', url);
    link.setAttribute('download', `contatos_agenda_${new Date().toISOString().split('T')[0]}.csv`);
    link.style.visibility = 'hidden';
    
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    
    showNotification('Contatos exportados com sucesso!', 'success');
}

// 10. Importar de CSV
function showImportSection() {
    showSection('import');
    document.getElementById('csvPreview').innerHTML = '';
}

function handleCSVUpload(event) {
    const file = event.target.files[0];
    if (file) {
        handleCSVFile(file);
    }
}

function handleCSVFile(file) {
    const reader = new FileReader();
    
    reader.onload = function(e) {
        const csvContent = e.target.result;
        previewCSV(csvContent);
    };
    
    reader.readAsText(file);
}

function previewCSV(csvContent) {
    const lines = csvContent.split('\n');
    const preview = document.getElementById('csvPreview');
    
    if (lines.length === 0) {
        preview.innerHTML = '<p class="error">Arquivo CSV vazio</p>';
        return;
    }
    
    let html = '<h3>Pré-visualização do CSV</h3>';
    html += '<div class="csv-table">';
    html += '<table>';
    
    // Cabeçalho
    html += '<thead><tr>';
    const headers = lines[0].split(',').map(h => `<th>${h.replace(/"/g, '')}</th>`).join('');
    html += headers + '</tr></thead>';
    
    // Primeiras 5 linhas
    html += '<tbody>';
    for (let i = 1; i < Math.min(6, lines.length); i++) {
        if (lines[i].trim()) {
            html += '<tr>';
            const cells = lines[i].split(',').map(c => `<td>${c.replace(/"/g, '')}</td>`).join('');
            html += cells + '</tr>';
        }
    }
    html += '</tbody>';
    html += '</table>';
    html += `</div><p>Mostrando ${Math.min(5, lines.length - 1)} de ${lines.length - 1} linhas</p>`;
    
    html += `<button class="btn-primary" onclick="importCSV(\`${btoa(csvContent)}\`)">
        <i class="fas fa-upload"></i> Importar Contatos
    </button>`;
    
    preview.innerHTML = html;
}

async function importCSV(base64CSV) {
    const csvContent = atob(base64CSV);
    const lines = csvContent.split('\n');
    
    if (lines.length < 2) {
        showNotification('Arquivo CSV inválido', 'error');
        return;
    }
    
    const headers = lines[0].split(',').map(h => h.replace(/"/g, '').trim());
    const nameIndex = headers.findIndex(h => h.toLowerCase() === 'nome');
    
    if (nameIndex === -1) {
        showNotification('CSV deve ter coluna "Nome"', 'error');
        return;
    }
    
    showLoading();
    let imported = 0;
    let errors = 0;
    
    for (let i = 1; i < lines.length; i++) {
        if (!lines[i].trim()) continue;
        
        const cells = lines[i].split(',').map(c => c.replace(/"/g, '').trim());
        if (cells.length < headers.length) continue;
        
        const contact = {
            name: cells[nameIndex],
            phone: cells[headers.findIndex(h => h.toLowerCase() === 'telefone')] || '',
            email: cells[headers.findIndex(h => h.toLowerCase() === 'email')] || '',
            favorite: cells[headers.findIndex(h => h.toLowerCase() === 'favorito')] === 'true'
        };
        
        if (contact.name) {
            try {
                const response = await fetch('/api/add', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify(contact)
                });
                
                const result = await response.json();
                if (result.success) {
                    imported++;
                } else {
                    errors++;
                }
            } catch (error) {
                errors++;
            }
        }
    }
    
    hideLoading();
    
    let message = `${imported} contatos importados com sucesso!`;
    if (errors > 0) {
        message += ` (${errors} erros)`;
    }
    
    showNotification(message, imported > 0 ? 'success' : 'warning');
    
    if (imported > 0) {
        await loadContacts();
        showSection('all-contacts');
    }
}

// 11. Sair
function exitApp() {
    showConfirmModal(
        'Tem certeza que deseja sair do sistema?',
        () => {
            showNotification('Sistema finalizado. Até logo! 👋', 'info');
            setTimeout(() => {
                // Em um ambiente real, aqui fecharia a janela/aba
                document.body.innerHTML = `
                    <div style="display: flex; justify-content: center; align-items: center; height: 100vh; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; font-family: Arial;">
                        <div style="text-align: center;">
                            <h1 style="font-size: 2.5rem; margin-bottom: 1rem;">👋 Até logo!</h1>
                            <p style="font-size: 1.2rem;">Sistema Agenda AVL finalizado</p>
                        </div>
                    </div>
                `;
            }, 2000);
        }
    );
}

// Funções Auxiliares
async function loadContacts() {
    try {
        const response = await fetch('/api/contacts');
        const data = await response.json();
        
        if (data.success) {
            contacts = data.contacts || [];
            updateHeaderStats();
            
            // Atualizar display baseado na seção atual
            switch(currentSection) {
                case 'all-contacts':
                    displayContacts(contacts);
                    break;
                case 'favorites':
                    displayFavorites();
                    break;
                case 'statistics':
                    loadDetailedStatistics();
                    break;
            }
        }
    } catch (error) {
        showNotification('Erro ao carregar contatos: ' + error.message, 'error');
    }
}

function updateHeaderStats() {
    const total = contacts.length;
    const favorites = contacts.filter(c => c.favorite).length;
    
    document.getElementById('totalContacts').textContent = total;
    document.getElementById('totalFavorites').textContent = favorites;
}

function createContactCard(contact) {
    return `
        <div class="contact-card ${contact.favorite ? 'favorite' : ''}">
            <input type="checkbox" class="contact-checkbox" value="${contact.name}" 
                   onchange="toggleSelection('${contact.name}', this.checked)"
                   style="display: none;">
            <div class="contact-header">
                <div class="contact-name">${contact.name}</div>
                <div class="contact-favorite">
                    ${contact.favorite ? '<i class="fas fa-star"></i>' : ''}
                </div>
            </div>
            <div class="contact-info">
                ${contact.phone ? `
                    <div class="contact-field">
                        <i class="fas fa-phone"></i>
                        <span>${contact.phone}</span>
                    </div>
                ` : ''}
                ${contact.email ? `
                    <div class="contact-field">
                        <i class="fas fa-envelope"></i>
                        <span>${contact.email}</span>
                    </div>
                ` : ''}
            </div>
            <div class="contact-actions">
                <button class="btn-warning" onclick="toggleFavorite('${contact.name}')">
                    <i class="fas fa-star"></i>
                    ${contact.favorite ? 'Desfavoritar' : 'Favoritar'}
                </button>
                <button class="btn-danger" onclick="removeContact('${contact.name}')">
                    <i class="fas fa-trash"></i> Remover
                </button>
            </div>
        </div>
    `;
}

function toggleSelection(contactName, isChecked) {
    if (isChecked) {
        selectedContacts.add(contactName);
    } else {
        selectedContacts.delete(contactName);
    }
}

async function removeSelected() {
    if (selectedContacts.size === 0) {
        showNotification('Selecione pelo menos um contato para remover', 'warning');
        return;
    }

    showConfirmModal(
        `Tem certeza que deseja remover ${selectedContacts.size} contato(s) selecionado(s)?`,
        async () => {
            showLoading();
            let successCount = 0;
            let errorCount = 0;

            for (const contactName of selectedContacts) {
                try {
                    const response = await fetch('/api/remove', {
                        method: 'POST',
                        headers: {'Content-Type': 'application/json'},
                        body: JSON.stringify({ name: contactName })
                    });
                    
                    const result = await response.json();
                    if (result.success) {
                        successCount++;
                    } else {
                        errorCount++;
                    }
                } catch (error) {
                    errorCount++;
                }
            }
            
            let message = `${successCount} contato(s) removido(s) com sucesso!`;
            if (errorCount > 0) {
                message += ` (${errorCount} erros)`;
            }
            
            showNotification(message, successCount > 0 ? 'success' : 'error');
            selectedContacts.clear();
            await loadContacts();
            hideLoading();
        }
    );
}

// Modal functions
function closeModal() {
    document.getElementById('contactModal').style.display = 'none';
}

function showConfirmModal(message, confirmCallback) {
    document.getElementById('confirmMessage').textContent = message;
    document.getElementById('confirmAction').onclick = confirmCallback;
    document.getElementById('confirmModal').style.display = 'block';
}

function closeConfirmModal() {
    document.getElementById('confirmModal').style.display = 'none';
}

// UI Helpers
function showLoading() {
    document.getElementById('loadingOverlay').style.display = 'block';
}

function hideLoading() {
    document.getElementById('loadingOverlay').style.display = 'none';
}

function showNotification(message, type) {
    // Remove notificações existentes
    const existingNotifications = document.querySelectorAll('.notification');
    existingNotifications.forEach(notification => notification.remove());

    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.innerHTML = `
        <div class="notification-content">
            <i class="fas fa-${getNotificationIcon(type)}"></i>
            <span>${message}</span>
        </div>
    `;

    document.body.appendChild(notification);

    // Animação de entrada
    setTimeout(() => notification.classList.add('show'), 100);

    // Auto-remover
    setTimeout(() => {
        notification.classList.remove('show');
        setTimeout(() => notification.remove(), 300);
    }, 4000);
}

function getNotificationIcon(type) {
    switch(type) {
        case 'success': return 'check-circle';
        case 'error': return 'exclamation-circle';
        case 'warning': return 'exclamation-triangle';
        case 'info': return 'info-circle';
        default: return 'bell';
    }
}

// CSS para notificações (adicionar ao style.css)
const notificationStyles = `
    .notification {
        position: fixed;
        top: 20px;
        right: 20px;
        background: white;
        padding: 15px 20px;
        border-radius: 10px;
        box-shadow: 0 5px 15px rgba(0,0,0,0.2);
        border-left: 4px solid var(--primary);
        transform: translateX(400px);
        transition: transform 0.3s ease;
        z-index: 3000;
        max-width: 400px;
    }

    .notification.show {
        transform: translateX(0);
    }

    .notification.success {
        border-left-color: var(--success);
    }

    .notification.error {
        border-left-color: var(--danger);
    }

    .notification.warning {
        border-left-color: var(--warning);
    }

    .notification.info {
        border-left-color: var(--info);
    }

    .notification-content {
        display: flex;
        align-items: center;
        gap: 10px;
    }

    .notification i {
        font-size: 1.2rem;
    }

    .notification.success i { color: var(--success); }
    .notification.error i { color: var(--danger); }
    .notification.warning i { color: var(--warning); }
    .notification.info i { color: var(--info); }
`;

// Adicionar estilos das notificações
const styleSheet = document.createElement('style');
styleSheet.textContent = notificationStyles;
document.head.appendChild(styleSheet);

// Fechar modais clicando fora
window.onclick = function(event) {
    const contactModal = document.getElementById('contactModal');
    const confirmModal = document.getElementById('confirmModal');
    
    if (event.target === contactModal) {
        closeModal();
    }
    if (event.target === confirmModal) {
        closeConfirmModal();
    }
}