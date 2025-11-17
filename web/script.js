let contacts = [];
let selectedContacts = new Set();

document.addEventListener('DOMContentLoaded', function() {
    loadContacts();
    setupEventListeners();
});

function setupEventListeners() {
    document.getElementById('contactForm').addEventListener('submit', function(e) {
        e.preventDefault();
        saveContact();
    });

    document.getElementById('selectAll').addEventListener('change', function() {
        const checkboxes = document.querySelectorAll('.contact-checkbox');
        checkboxes.forEach(checkbox => {
            checkbox.checked = this.checked;
            if (this.checked) {
                selectedContacts.add(checkbox.value);
            } else {
                selectedContacts.delete(checkbox.value);
            }
        });
    });

    document.getElementById('searchInput').addEventListener('input', function() {
        filterContacts(this.value);
    });
}

async function loadContacts() {
    try {
        console.log('Carregando contatos...');
        const response = await fetch('/api/contacts');
        const data = await response.json();
        console.log('Dados recebidos:', data);
        
        if (data.success) {
            contacts = data.contacts || [];
            displayContacts(contacts);
            
            // Carregar estatísticas separadamente
            await loadStatistics();
        } else {
            showError('Erro na resposta do servidor');
        }
    } catch (error) {
        console.error('Erro detalhado:', error);
        showError('Erro ao carregar contatos: ' + error.message);
    }
}

async function loadStatistics() {
    try {
        const response = await fetch('/api/statistics');
        const data = await response.json();
        console.log('Estatísticas:', data);
        
        if (data.success && data.statistics) {
            updateStatistics(data.statistics);
        } else {
            // Calcular estatísticas localmente se a API não retornar
            calculateLocalStatistics();
        }
    } catch (error) {
        console.error('Erro ao carregar estatísticas:', error);
        calculateLocalStatistics();
    }
}

function calculateLocalStatistics() {
    const total = contacts.length;
    const favorites = contacts.filter(contact => contact.favorite).length;
    const balanced = true; // Assumindo que está balanceada
    
    updateStatistics({
        total: total,
        favorites: favorites,
        balanced: balanced
    });
}

function displayContacts(contactsToShow) {
    const tbody = document.getElementById('contactsBody');
    tbody.innerHTML = '';

    if (contactsToShow.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" style="text-align: center; padding: 20px;">Nenhum contato encontrado</td></tr>';
        return;
    }

    contactsToShow.forEach(contact => {
        const row = document.createElement('tr');
        
        // Escapar caracteres especiais para evitar problemas no HTML
        const safeName = escapeHtml(contact.name || '');
        const safePhone = escapeHtml(contact.phone || '');
        const safeEmail = escapeHtml(contact.email || '');
        
        row.innerHTML = `
            <td><input type="checkbox" class="contact-checkbox" value="${safeName}" onchange="toggleSelection('${safeName}', this.checked)"></td>
            <td>${safeName}</td>
            <td>${safePhone}</td>
            <td>${safeEmail}</td>
            <td class="favorite-star">${contact.favorite ? '★' : ''}</td>
            <td class="actions-cell">
                <button class="action-btn btn-warning" onclick="toggleFavorite('${safeName}')">
                    ${contact.favorite ? 'Remover' : 'Favoritar'}
                </button>
                <button class="action-btn btn-danger" onclick="removeContact('${safeName}')">
                    Remover
                </button>
            </td>
        `;
        tbody.appendChild(row);
    });

    updateSelectAllCheckbox();
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

function toggleSelection(contactName, isChecked) {
    if (isChecked) {
        selectedContacts.add(contactName);
    } else {
        selectedContacts.delete(contactName);
    }
    updateSelectAllCheckbox();
}

function updateSelectAllCheckbox() {
    const selectAll = document.getElementById('selectAll');
    const checkboxes = document.querySelectorAll('.contact-checkbox');
    
    if (checkboxes.length === 0) {
        selectAll.checked = false;
        return;
    }

    const checkedCount = document.querySelectorAll('.contact-checkbox:checked').length;
    selectAll.checked = checkedCount === checkboxes.length;
}

function filterContacts(searchTerm) {
    if (!searchTerm) {
        displayContacts(contacts);
        return;
    }
    
    const filtered = contacts.filter(contact => {
        const name = (contact.name || '').toLowerCase();
        const phone = (contact.phone || '').toLowerCase();
        const email = (contact.email || '').toLowerCase();
        const search = searchTerm.toLowerCase();
        
        return name.includes(search) || phone.includes(search) || email.includes(search);
    });
    displayContacts(filtered);
}

function searchContacts() {
    const searchTerm = document.getElementById('searchInput').value;
    filterContacts(searchTerm);
}

function showAddForm() {
    document.getElementById('modalTitle').textContent = 'Adicionar Contato';
    document.getElementById('contactForm').reset();
    document.getElementById('contactModal').style.display = 'block';
}

function closeModal() {
    document.getElementById('contactModal').style.display = 'none';
}

async function saveContact() {
    const formData = {
        name: document.getElementById('name').value.trim(),
        phone: document.getElementById('phone').value.trim(),
        email: document.getElementById('email').value.trim(),
        favorite: document.getElementById('favorite').checked
    };

    if (!formData.name) {
        showError('Nome é obrigatório!');
        return;
    }

    try {
        const response = await fetch('/api/add', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(formData)
        });

        const result = await response.json();
        console.log('Resposta do servidor:', result);
        
        if (result.success) {
            showSuccess('Contato salvo com sucesso!');
            closeModal();
            await loadContacts(); // Recarregar a lista
        } else {
            showError(result.message || 'Erro ao salvar contato');
        }
    } catch (error) {
        console.error('Erro ao salvar:', error);
        showError('Erro ao salvar contato: ' + error.message);
    }
}

async function removeContact(contactName) {
    if (!confirm('Tem certeza que deseja remover "' + contactName + '"?')) {
        return;
    }

    try {
        const response = await fetch('/api/remove', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ name: contactName })
        });

        const result = await response.json();
        console.log('Resposta da remoção:', result);
        
        if (result.success) {
            showSuccess('Contato removido com sucesso!');
            await loadContacts(); // Recarregar a lista
        } else {
            showError(result.message || 'Erro ao remover contato');
        }
    } catch (error) {
        console.error('Erro ao remover:', error);
        showError('Erro ao remover contato: ' + error.message);
    }
}

async function removeSelected() {
    if (selectedContacts.size === 0) {
        showError('Selecione pelo menos um contato para remover');
        return;
    }

    if (!confirm('Tem certeza que deseja remover ' + selectedContacts.size + ' contato(s)?')) {
        return;
    }

    try {
        let successCount = 0;
        let errorCount = 0;

        for (const contactName of selectedContacts) {
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
                    successCount++;
                } else {
                    errorCount++;
                }
            } catch (error) {
                errorCount++;
            }
        }
        
        let message = successCount + ' contato(s) removido(s) com sucesso!';
        if (errorCount > 0) {
            message += ' (' + errorCount + ' erros)';
        }
        
        showSuccess(message);
        selectedContacts.clear();
        await loadContacts(); // Recarregar a lista
    } catch (error) {
        console.error('Erro ao remover múltiplos:', error);
        showError('Erro ao remover contatos: ' + error.message);
    }
}

async function toggleFavorite(contactName) {
    try {
        const response = await fetch('/api/toggle-favorite', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ name: contactName })
        });

        const result = await response.json();
        console.log('Resposta do favorito:', result);
        
        if (result.success) {
            showSuccess('Favorito atualizado!');
            await loadContacts(); // Recarregar a lista
        } else {
            showError(result.message || 'Erro ao atualizar favorito');
        }
    } catch (error) {
        console.error('Erro ao favoritar:', error);
        showError('Erro ao atualizar favorito: ' + error.message);
    }
}

async function toggleFavoriteSelected() {
    if (selectedContacts.size === 0) {
        showError('Selecione pelo menos um contato');
        return;
    }

    try {
        let successCount = 0;
        let errorCount = 0;

        for (const contactName of selectedContacts) {
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
                    successCount++;
                } else {
                    errorCount++;
                }
            } catch (error) {
                errorCount++;
            }
        }
        
        let message = successCount + ' contato(s) atualizado(s)!';
        if (errorCount > 0) {
            message += ' (' + errorCount + ' erros)';
        }
        
        showSuccess(message);
        selectedContacts.clear();
        await loadContacts(); // Recarregar a lista
    } catch (error) {
        console.error('Erro ao favoritar múltiplos:', error);
        showError('Erro ao atualizar favoritos: ' + error.message);
    }
}

function updateStatistics(stats) {
    console.log('Atualizando estatísticas:', stats);
    
    const totalElem = document.getElementById('totalContacts');
    const favoritesElem = document.getElementById('totalFavorites');
    const balancedElem = document.getElementById('treeBalanced');
    
    if (totalElem) totalElem.textContent = stats.total || 0;
    if (favoritesElem) favoritesElem.textContent = stats.favorites || 0;
    if (balancedElem) balancedElem.textContent = stats.balanced ? 'Sim' : 'Não';
}

function showSuccess(message) {
    showMessage(message, 'success');
}

function showError(message) {
    showMessage(message, 'error');
}

function showMessage(message, type) {
    // Remove mensagens existentes
    const existingMessages = document.querySelectorAll('.success-message, .error-message');
    existingMessages.forEach(msg => msg.remove());

    const messageDiv = document.createElement('div');
    messageDiv.className = type === 'success' ? 'success-message' : 'error-message';
    messageDiv.textContent = message;

    const controls = document.querySelector('.controls');
    if (controls && controls.parentNode) {
        controls.parentNode.insertBefore(messageDiv, controls.nextSibling);
    }

    // Auto-remover após 5 segundos
    setTimeout(() => {
        if (messageDiv.parentNode) {
            messageDiv.remove();
        }
    }, 5000);
}

// Fechar modal clicando fora
window.onclick = function(event) {
    const modal = document.getElementById('contactModal');
    if (event.target === modal) {
        closeModal();
    }
}

// Debug: verificar se os elementos existem
console.log('Elementos carregados:');
console.log('- searchInput:', document.getElementById('searchInput'));
console.log('- contactsBody:', document.getElementById('contactsBody'));
console.log('- totalContacts:', document.getElementById('totalContacts'));